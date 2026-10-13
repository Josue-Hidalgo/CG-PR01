#!/usr/bin/env python3
"""
Convierte un GeoJSON de las provincias de Costa Rica (geoBoundaries, GADM,
Natural Earth, OpenStreetMap, IGN...) al formato que lee data.c: un feature
por provincia con properties.id = CRSJ | CRA | CRC | CRH | CRG | CRP | CRL.

Uso:
    python3 conv/converter.py assets/geoBoundaries-CRI-ADM1.geojson assets/cr.json

Notas:
  * Une los features de una misma provincia (p. ej. las islas) en un MultiPolygon.
  * data.c solo lee el primer anillo de cada poligono, asi que los huecos se
    descartan (el script avisa cuantos).
  * Redondea a 6 decimales (~0.1 m) y quita puntos consecutivos repetidos.
  * Si el archivo viene en otro sistema de coordenadas (metros, CRTM05) hay que
    reproyectarlo antes a WGS84:  ogr2ogr -f GeoJSON -t_srs EPSG:4326 out.geojson in.shp
"""

import argparse
import json
import sys
import unicodedata

PROVINCIAS = {  # nombre sin tildes en minuscula -> id que espera data.c
    "san jose": "CRSJ", "alajuela": "CRA", "cartago": "CRC", "heredia": "CRH",
    "guanacaste": "CRG", "puntarenas": "CRP", "limon": "CRL",
}
NOMBRES = {"CRSJ": "San José", "CRA": "Alajuela", "CRC": "Cartago", "CRH": "Heredia",
           "CRG": "Guanacaste", "CRP": "Puntarenas", "CRL": "Limón"}
ISO = {"CR-SJ": "CRSJ", "CR-A": "CRA", "CR-C": "CRC", "CR-H": "CRH",
       "CR-G": "CRG", "CR-P": "CRP", "CR-L": "CRL"}


def normalizar(s):
    s = unicodedata.normalize("NFD", str(s))
    return "".join(c for c in s if unicodedata.category(c) != "Mn").strip().lower()


def provincia_de(props, campo=None):
    """Busca la provincia en las propiedades (o solo en `campo` si se indica)."""
    valores = [props.get(campo)] if campo else list(props.values())
    for v in valores:
        if not isinstance(v, str):
            continue
        if v.strip().upper() in ISO:
            return ISO[v.strip().upper()]
        n = normalizar(v)
        if n in PROVINCIAS:
            return PROVINCIAS[n]
    return None


def limpiar_anillo(anillo):
    pts = []
    for x, y in ((p[0], p[1]) for p in anillo):
        q = [round(x, 6), round(y, 6)]
        if not pts or q != pts[-1]:
            pts.append(q)
    if len(pts) > 1 and pts[0] == pts[-1]:
        pts.pop()
    if len(pts) < 3:
        return None
    pts.append(pts[0])  # cerrado, como pide GeoJSON
    return pts


def poligonos(geom):
    if geom["type"] == "Polygon":
        return [geom["coordinates"]]
    if geom["type"] == "MultiPolygon":
        return geom["coordinates"]
    return []


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("entrada")
    ap.add_argument("salida")
    ap.add_argument("--campo", help="propiedad con el nombre/codigo de la provincia (por defecto se prueban todas)")
    ap.add_argument("--min-vertices", type=int, default=3, help="descartar poligonos con menos vertices (def. 3)")
    a = ap.parse_args()

    datos = json.load(open(a.entrada, encoding="utf-8"))
    features = datos["features"] if datos.get("type") == "FeatureCollection" else [datos]

    por_id = {}
    sin_clasificar = []
    huecos = descartados = 0
    for f in features:
        pid = provincia_de(f.get("properties") or {}, a.campo)
        if pid is None:
            sin_clasificar.append(f.get("properties"))
            continue
        for poly in poligonos(f["geometry"]):
            huecos += len(poly) - 1
            anillo = limpiar_anillo(poly[0])
            if anillo is None or len(anillo) - 1 < a.min_vertices:
                descartados += 1
                continue
            por_id.setdefault(pid, []).append([anillo])

    if sin_clasificar and len(por_id) < 7:
        print("No pude identificar la provincia de estos features:", file=sys.stderr)
        for p in sin_clasificar[:10]:
            print("  ", p, file=sys.stderr)
        print("Usa --campo NOMBRE_DE_LA_PROPIEDAD", file=sys.stderr)
        sys.exit(1)
    faltan = [i for i in NOMBRES if i not in por_id]
    if faltan:
        print("Faltan provincias:", faltan, file=sys.stderr)
        sys.exit(1)

    # data.c busca el PRIMER "id" del feature: properties.id va primero y no hay id a nivel feature
    salida = []
    for pid in NOMBRES:
        polys = por_id[pid]
        geom = ({"type": "Polygon", "coordinates": polys[0]} if len(polys) == 1
                else {"type": "MultiPolygon", "coordinates": polys})
        salida.append({"type": "Feature", "properties": {"id": pid, "name": NOMBRES[pid]}, "geometry": geom})

    with open(a.salida, "w", encoding="utf-8") as fh:
        fh.write('{"type": "FeatureCollection", "features": [\n')
        fh.write(",\n".join(json.dumps(f, ensure_ascii=False) for f in salida))
        fh.write("\n]}\n")

    # resumen y chequeo de bordes compartidos (el arreglo de bordes del render depende de esto)
    duenos = {}
    total = 0
    print(f"{'provincia':<12}{'poligonos':>10}{'vertices':>10}")
    for pid in NOMBRES:
        nv = 0
        for poly in por_id[pid]:
            for x, y in poly[0][:-1]:
                duenos.setdefault((x, y), set()).add(pid)
                nv += 1
        total += nv
        print(f"{NOMBRES[pid]:<12}{len(por_id[pid]):>10}{nv:>10}")
    compartidos = sum(1 for v in duenos.values() if len(v) > 1)
    print(f"total vertices: {total}   vertices compartidos exactos entre provincias: {compartidos}")
    if huecos:
        print(f"aviso: {huecos} huecos descartados (data.c no los lee)")
    if descartados:
        print(f"aviso: {descartados} poligonos descartados por tener menos de {a.min_vertices} vertices")
    if compartidos < 100:
        print("AVISO: casi no hay vertices compartidos; los limites entre provincias no son "
              "topologicamente consistentes y el borde compartido puede verse doble o con mezcla de colores.")


if __name__ == "__main__":
    main()
