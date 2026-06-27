#!/usr/bin/env python3
"""Generate the Pelikone A4 user manual (Finnish) as a PDF.

Run:  python3 tools/gen_manual.py
Output: Pelikone_Kayttoohje.pdf in the project root.
"""
import os
from reportlab.lib.pagesizes import A4
from reportlab.lib.units import cm
from reportlab.lib import colors
from reportlab.lib.styles import getSampleStyleSheet, ParagraphStyle
from reportlab.lib.enums import TA_LEFT
from reportlab.platypus import (SimpleDocTemplate, Paragraph, Spacer, Table,
                                TableStyle, PageBreak)

HERE = os.path.dirname(os.path.abspath(__file__))
OUT = os.path.join(HERE, "..", "Pelikone_Kayttoohje.pdf")

# --- Palette ---
INK   = colors.HexColor("#1A1A1A")
TEAL  = colors.HexColor("#11607A")   # headings / accent
TEALD = colors.HexColor("#0C4456")
GREY  = colors.HexColor("#666666")
LINE  = colors.HexColor("#CDD6DA")
ROW   = colors.HexColor("#EEF4F6")   # alt table row
WARNBG = colors.HexColor("#FCEAEA")  # battery box
WARNBD = colors.HexColor("#C0392B")  # battery border / title

styles = getSampleStyleSheet()

def S(name, **kw):
    base = kw.pop("parent", styles["Normal"])
    return ParagraphStyle(name, parent=base, **kw)

st_title = S("t", fontName="Helvetica-Bold", fontSize=21, textColor=TEALD, leading=24)
st_sub   = S("s", fontName="Helvetica", fontSize=10.5, textColor=GREY, leading=13)
st_body  = S("b", fontName="Helvetica", fontSize=9.5, textColor=INK, leading=13)
st_bodyb = S("bb", fontName="Helvetica-Bold", fontSize=9.5, textColor=INK, leading=13)
st_h2    = S("h2", fontName="Helvetica-Bold", fontSize=12.5, textColor=colors.white, leading=15)
st_warnt = S("wt", fontName="Helvetica-Bold", fontSize=12.5, textColor=WARNBD, leading=15)
st_warn  = S("w", fontName="Helvetica", fontSize=9.5, textColor=INK, leading=13)
st_cellh = S("ch", fontName="Helvetica-Bold", fontSize=9, textColor=colors.white, leading=11)
st_game  = S("g", fontName="Helvetica-Bold", fontSize=9, textColor=TEALD, leading=11)
st_cell  = S("c", fontName="Helvetica", fontSize=8.6, textColor=INK, leading=10.5)
st_foot  = S("f", fontName="Helvetica-Oblique", fontSize=8, textColor=GREY, leading=10)

# ============================================================================
# GAME LIST — to add a new game, add ONE (name, idea, controls) tuple below in
# the same order as the device menu, then run:  python3 tools/gen_manual.py
# You can type Finnish letters directly (ä ö) or use &auml; &ouml; &Auml; &Ouml;.
# The "N games" count in the manual updates itself from this list.
# ============================================================================
games = [
    ("SANULI", "Arvaa 5-kirjaiminen sana kuudella yrityksell&auml;.",
     "Kirjoita kirjaimet (&Auml;=piste, &Ouml;=pilkku), ENTER arvaa, askelpalautin poistaa. "
     "Vihre&auml;=oikea paikka, keltainen=v&auml;&auml;r&auml; paikka, harmaa=ei sanassa."),
    ("MATOPELI", "Klassinen mato: sy&ouml; ruokaa ja kasva, &auml;l&auml; osu sein&auml;&auml;n tai itseesi.",
     "Nuolet k&auml;&auml;nt&auml;v&auml;t matoa."),
    ("2048", "Yhdist&auml; samat luvut. Tavoite: 2048.",
     "Nuolet liu'uttavat kaikkia laattoja."),
    ("RISTINOLLA", "Kolme suoraan voittaa. Kaksi pelaajaa (X ja O) vuorotellen.",
     "Nuolet liikuttavat, ENTER asettaa merkin."),
    ("MIINAHARAVA", "Paljasta kaikki ruudut osumatta miinaan. Numerot kertovat l&auml;himiinat.",
     "Nuolet liikuttavat, ENTER paljastaa, V&Auml;LILY&Ouml;NTI tai L asettaa lipun."),
    ("FLAPPY BIRD", "Lenn&auml;t&auml; lintua putkien aukoista.",
     "Yl&ouml;s-nuoli r&auml;p&auml;ytt&auml;&auml; siipi&auml;."),
    ("SHAKKI", "Tavallinen shakki: valitse alussa <b>2 PELAAJAA</b> (yksi laite) tai <b>2 LAITETTA</b> (langaton).",
     "Nuolet liikuttavat. ENTER valitsee nappulan, sitten kohderuudun (sama ruutu peruu valinnan)."),
    ("BLACKJACK", "Yrit&auml; saada 21, mutta &auml;l&auml; mene yli.",
     "H = ota kortti (hit), S = j&auml;&auml; (stand)."),
    ("SOKOBAN", "Ty&ouml;nn&auml; laatikot merkityille paikoille. 10 tasoa.",
     "Nuolet liikuttavat, U peruu siirron, R aloittaa tason alusta, ENTER siirtyy seuraavaan tasoon."),
    ("TETRIS", "Sovita putoavat palikat ja t&auml;yt&auml; rivit.",
     "Vasen/oikea liikuttaa, yl&ouml;s k&auml;&auml;nt&auml;&auml;, alas pudottaa pehme&auml;sti, V&Auml;LILY&Ouml;NTI pudottaa heti."),
    ("GEODASH", "Hyppele esteiden yli. Tilat: INFINITE ja LEVEL 1&ndash;3 (paras % tallentuu).",
     "Yl&ouml;s-nuoli tai V&Auml;LILY&Ouml;NTI hypp&auml;&auml;. Valikossa yl&ouml;s/alas ja ENTER."),
    ("RALLI F1", "V&auml;ist&auml; vastaantulevia autoja kaistoilla.",
     "Vasen/oikea-nuoli vaihtaa kaistaa."),
    ("T&Auml;HT&Auml;&Auml;", "T&auml;ht&auml;&auml; ja ammu avaruusoliot ennen kuin aika loppuu.",
     "Nuolet liikuttavat t&auml;ht&auml;int&auml;, ENTER ampuu."),
    ("HIRSIPUU", "Arvaa sana ennen kuin hirsipuu valmistuu. Satunnainen sana tai sy&ouml;t&auml; oma sana (2 pelaajaa).",
     "Kirjoita kirjaimet (&Auml;=piste, &Ouml;=pilkku). Valikossa yl&ouml;s/alas ja ENTER."),
    ("PAC-MAN", "Sy&ouml; kaikki pisteet ja v&auml;ist&auml; haamut.",
     "Nuolet ohjaavat Pac-Mania."),
    ("TIILISEIN&Auml;", "Kimmota palloa melalla ja riko kaikki tiilet. 3 el&auml;m&auml;&auml;.",
     "Vasen/oikea-nuoli liikuttaa melaa."),
    ("NELJ&Auml;N SUORA", "Pudota kiekkoja ja saa nelj&auml; suoraan (vaaka, pysty tai vino). "
     "Valitse alussa konetta vastaan <b>HELPPO</b> tai <b>VAIKEA</b>, tai <b>2 PELAAJAA</b> (punainen vs. keltainen).",
     "Valikossa yl&ouml;s/alas + ENTER valitsee. Pelissä vasen/oikea valitsee sarakkeen, "
     "ENTER tai alas-nuoli pudottaa kiekon."),
]

story = []

def section_bar(text, bg=TEAL, style=st_h2):
    t = Table([[Paragraph(text, style)]], colWidths=[18.0 * cm])
    t.setStyle(TableStyle([
        ("BACKGROUND", (0, 0), (-1, -1), bg),
        ("LEFTPADDING", (0, 0), (-1, -1), 8),
        ("RIGHTPADDING", (0, 0), (-1, -1), 8),
        ("TOPPADDING", (0, 0), (-1, -1), 4),
        ("BOTTOMPADDING", (0, 0), (-1, -1), 4),
    ]))
    return t

def bullets(items, style=st_body):
    data = [[Paragraph("&bull;", style), Paragraph(x, style)] for x in items]
    t = Table(data, colWidths=[0.45 * cm, 17.4 * cm])
    t.setStyle(TableStyle([
        ("VALIGN", (0, 0), (-1, -1), "TOP"),
        ("LEFTPADDING", (0, 0), (-1, -1), 0),
        ("RIGHTPADDING", (0, 0), (-1, -1), 0),
        ("TOPPADDING", (0, 0), (-1, -1), 1),
        ("BOTTOMPADDING", (0, 0), (-1, -1), 1),
    ]))
    return t

# ============================ HEADER ============================
story.append(Paragraph("PELIKONE &ndash; K&auml;ytt&ouml;ohje", st_title))
story.append(Paragraph(f"Pieni k&auml;sikonsoli, jossa on {len(games)} peli&auml;. "
                       "T&auml;m&auml; ohje: sivu 1 laitteen k&auml;ytt&ouml; ja akku, sivu 2 pelit.", st_sub))
story.append(Spacer(1, 8))

# ============================ HOW IT WORKS ============================
story.append(section_bar("N&auml;in laite toimii"))
story.append(Spacer(1, 4))
story.append(bullets([
    "Laitteessa on v&auml;rin&auml;ytt&ouml;, n&auml;pp&auml;imist&ouml; ja ladattava akku.",
    "<b>Virtakytkin</b> kytkee virran p&auml;&auml;lle ja pois. Laite k&auml;ynnistyy <b>pelivalikkoon</b>.",
    "Valitse peli valikosta ja pelaa. P&auml;&auml;set aina takaisin valikkoon <b>ESC</b>-n&auml;pp&auml;imell&auml;.",
    "Parhaat tulokset, putket ja edistyminen <b>tallentuvat muistiin</b> &ndash; ne s&auml;ilyv&auml;t, vaikka virta katkaistaan.",
]))
story.append(Spacer(1, 8))

# ============================ CONTROLS ============================
story.append(section_bar("Perusn&auml;pp&auml;imet (samat kaikkialla)"))
story.append(Spacer(1, 4))
ctl = [
    ("Nuolin&auml;pp&auml;imet", "Liiku ja ohjaa (vasen / oikea / yl&ouml;s / alas)."),
    ("ENTER", "Valitse, vahvista ja aloita peli alusta, kun peli on loppunut."),
    ("ESC", "Palaa pelivalikkoon &ndash; toimii jokaisessa peliss&auml; milloin vain."),
    ("TAB", "N&auml;yt&auml; akun tila (j&auml;nnite ja varausprosentti)."),
    ("Valikossa", "Yl&ouml;s / alas selaa pelej&auml;, ENTER aloittaa valitun pelin."),
    ("Sanapelit", "Kirjoita kirjaimet. <b>&Auml; = piste (.)</b>, <b>&Ouml; = pilkku (,)</b>. Askelpalautin poistaa kirjaimen."),
]
data = [[Paragraph(k, st_bodyb), Paragraph(v, st_body)] for k, v in ctl]
tctl = Table(data, colWidths=[3.6 * cm, 14.4 * cm])
tctl.setStyle(TableStyle([
    ("VALIGN", (0, 0), (-1, -1), "TOP"),
    ("LEFTPADDING", (0, 0), (-1, -1), 0),
    ("TOPPADDING", (0, 0), (-1, -1), 1.5),
    ("BOTTOMPADDING", (0, 0), (-1, -1), 1.5),
    ("LINEBELOW", (0, 0), (-1, -2), 0.4, LINE),
]))
story.append(tctl)
story.append(Spacer(1, 10))

# ============================ BATTERY (highlight box) ============================
batt_inner = []
batt_inner.append(Paragraph("Akku ja lataus &ndash; t&auml;rke&auml;&auml;", st_warnt))
batt_inner.append(Spacer(1, 3))
batt_inner.append(bullets([
    "<b>Lataus:</b> lataa USB-kaapelilla (esim. puhelimen laturi, 5 V) &ndash; samalla kaapelilla kuin laite ohjelmoidaan.",
    "<b>Turvallinen alue:</b> akku on litiumakku. T&auml;ysi on noin <b>4,2 V (100 %)</b> ja tyhj&auml; noin <b>3,0 V (0 %)</b>. "
    "&Auml;l&auml; p&auml;&auml;st&auml; akkua tyhjenem&auml;&auml;n aivan loppuun &auml;l&auml;k&auml; pid&auml; sit&auml; pitki&auml; aikoja t&auml;ynn&auml;.",
    "<b>Laite varoittaa itse:</b> kun varausta on <b>20 % tai alle</b>, n&auml;ytt&ouml;&ouml;n tulee punainen "
    "<b>&rdquo;AKKU V&Auml;HISS&Auml;! Lataa pian!&rdquo;</b> &ndash; varoitus toistuu noin minuutin v&auml;lein. Laita laite silloin lataukseen.",
    "<b>Lataa valvotusti:</b> &auml;l&auml; j&auml;t&auml; lataukseen pitkiksi ajoiksi ilman valvontaa (esim. yli y&ouml;n). Lataa huoneenl&auml;mm&ouml;ss&auml;, "
    "ei pakkasessa eik&auml; kuumassa (noin 0&ndash;45 &deg;C).",
    "<b>Sammuta virtakytkimest&auml;</b>, kun et pelaa &ndash; se s&auml;&auml;st&auml;&auml; akkua eniten.",
    "<b>Pitk&auml; s&auml;ilytys:</b> lataa akku noin puolilleen (~50 %) ennen pitk&auml;&auml; taukoa.",
    "<b>Varoitus:</b> jos akku turpoaa, kuumenee tai vaurioituu, lopeta k&auml;ytt&ouml; heti. &Auml;l&auml; puhkaise akkua.",
]))
box = Table([[batt_inner]], colWidths=[18.0 * cm])
box.setStyle(TableStyle([
    ("BACKGROUND", (0, 0), (-1, -1), WARNBG),
    ("BOX", (0, 0), (-1, -1), 1.2, WARNBD),
    ("LEFTPADDING", (0, 0), (-1, -1), 8),
    ("RIGHTPADDING", (0, 0), (-1, -1), 8),
    ("TOPPADDING", (0, 0), (-1, -1), 6),
    ("BOTTOMPADDING", (0, 0), (-1, -1), 6),
]))
story.append(box)
story.append(Spacer(1, 10))

# ============================ POWER SAVING ============================
story.append(section_bar("Automaattinen virrans&auml;&auml;st&ouml;"))
story.append(Spacer(1, 4))
story.append(bullets([
    "Jos laitteeseen <b>ei kosketa 15 minuuttiin</b>, n&auml;ytt&ouml; sammuu ja laite menee kevyeen lepotilaan. "
    "<b>Mik&auml; tahansa n&auml;pp&auml;in her&auml;tt&auml;&auml;</b> laitteen, ja peli jatkuu siit&auml; mihin j&auml;i.",
    "Jos laite on viel&auml; <b>30 minuuttia lis&auml;&auml;</b> koskematta, se menee syv&auml;uneen (k&auml;yt&auml;nn&ouml;ss&auml; sammuksiin). "
    "T&auml;ll&ouml;in her&auml;t&auml; se <b>virtakytkimell&auml;</b> (pois ja takaisin p&auml;&auml;lle).",
    "N&auml;m&auml; suojaavat akkua, jos laite j&auml;&auml; vahingossa p&auml;&auml;lle. Silti: muista sammuttaa virtakytkin pelaamisen j&auml;lkeen.",
]))
story.append(Spacer(1, 10))

# ============================ CONTACT ============================
contact_inner = [
    Paragraph("Kysymyksi&auml;, ongelmia tai peli-ideoita?", st_bodyb),
    Spacer(1, 2),
    Paragraph("Ota <b>matalalla kynnyksell&auml;</b> yhteytt&auml; Kalleen &ndash; h&auml;n auttaa mielell&auml;&auml;n!", st_body),
]
cbox = Table([[contact_inner]], colWidths=[18.0 * cm])
cbox.setStyle(TableStyle([
    ("BACKGROUND", (0, 0), (-1, -1), colors.HexColor("#EAF3F6")),
    ("BOX", (0, 0), (-1, -1), 1.0, TEAL),
    ("LEFTPADDING", (0, 0), (-1, -1), 8),
    ("RIGHTPADDING", (0, 0), (-1, -1), 8),
    ("TOPPADDING", (0, 0), (-1, -1), 6),
    ("BOTTOMPADDING", (0, 0), (-1, -1), 6),
]))
story.append(cbox)

story.append(PageBreak())

# ============================ GAMES (page 2) ============================
story.append(Paragraph("Pelit", st_title))
story.append(Paragraph(f"{len(games)} peli&auml;. Pelin loputtua <b>ENTER</b> aloittaa uudelleen, <b>ESC</b> palaa valikkoon.", st_sub))
story.append(Spacer(1, 6))

rows = [[Paragraph("Peli", st_cellh), Paragraph("Idea", st_cellh), Paragraph("Ohjaus", st_cellh)]]
for name, idea, ctrl in games:
    rows.append([Paragraph(name, st_game), Paragraph(idea, st_cell), Paragraph(ctrl, st_cell)])

tg = Table(rows, colWidths=[3.0 * cm, 7.2 * cm, 7.8 * cm], repeatRows=1)
ts = [
    ("BACKGROUND", (0, 0), (-1, 0), TEAL),
    ("VALIGN", (0, 0), (-1, -1), "TOP"),
    ("LEFTPADDING", (0, 0), (-1, -1), 5),
    ("RIGHTPADDING", (0, 0), (-1, -1), 5),
    ("TOPPADDING", (0, 0), (-1, -1), 3.5),
    ("BOTTOMPADDING", (0, 0), (-1, -1), 3.5),
    ("LINEBELOW", (0, 0), (-1, -1), 0.4, LINE),
    ("BOX", (0, 0), (-1, -1), 0.6, LINE),
]
for i in range(1, len(rows)):
    if i % 2 == 0:
        ts.append(("BACKGROUND", (0, i), (-1, i), ROW))
tg.setStyle(TableStyle(ts))
story.append(tg)
story.append(Spacer(1, 8))
story.append(Paragraph(
    "Vinkki: jos peli tuntuu juuttuneen tai haluat lopettaa, paina ESC &ndash; p&auml;&auml;set aina takaisin valikkoon. "
    "Akun tilan n&auml;et milloin vain TAB-n&auml;pp&auml;imell&auml;.", st_foot))

# ============================ BUILD ============================
doc = SimpleDocTemplate(OUT, pagesize=A4,
                        leftMargin=1.5 * cm, rightMargin=1.5 * cm,
                        topMargin=1.3 * cm, bottomMargin=1.3 * cm,
                        title="Pelikone - Kayttoohje", author="Pelikone")
doc.build(story)
print("Wrote", os.path.normpath(OUT))
