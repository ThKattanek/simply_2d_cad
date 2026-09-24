#!/usr/bin/env python3
import os
import subprocess
import sys

# Pfade relativ zum Skript-Standort ermitteln
SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
DOC_DIR = os.path.abspath(os.path.join(SCRIPT_DIR, "../manual"))

DE_MAIN = os.path.join(DOC_DIR, "de", "main.adoc")
EN_MAIN = os.path.join(DOC_DIR, "en", "main.adoc")
THEME_PATH = os.path.join(DOC_DIR, "pdf-theme.yml")

# Zielordner für die generierten PDFs (z.B. bin/doc)
OUT_DIR = os.path.abspath(os.path.join(SCRIPT_DIR, "../../bin/doc"))

def generate_pdf(src_file, out_pdf, lang):
    """Ruft asciidoctor-pdf für eine Hauptdatei auf."""
    if not os.path.exists(src_file):
        print(f"Hinweis: Datei '{src_file}' nicht gefunden. Überspringe {lang.upper()}-PDF.")
        return

    cmd = [
        "asciidoctor-pdf",
        "-a", f"lang={lang}",
        "-a", f"pdf-theme={THEME_PATH}",
        src_file,
        "-o", out_pdf
    ]

    print(f"Erzeuge {lang.upper()}-PDF: {out_pdf}")
    try:
        subprocess.run(cmd, check=True)
    except subprocess.CalledProcessError as e:
        print(f"Fehler beim Erzeugen der {lang.upper()}-PDF: {e}")
        sys.exit(1)
    except FileNotFoundError:
        print("Fehler: 'asciidoctor-pdf' wurde nicht gefunden. Bitte installiere es mit 'gem install asciidoctor-pdf'.")
        sys.exit(1)

def build_docs():
    os.makedirs(OUT_DIR, exist_ok=True)

    # 1. Deutsche PDF generieren
    generate_pdf(DE_MAIN, os.path.join(OUT_DIR, "simply_2d_cad_manual_de.pdf"), "de")

    # 2. Englische PDF generieren
    generate_pdf(EN_MAIN, os.path.join(OUT_DIR, "simply_2d_cad_manual_en.pdf"), "en")

    print(f"\nFertig! Die PDFs wurden unter '{OUT_DIR}' abgelegt.")

if __name__ == "__main__":
    build_docs()
