#ifndef GAME_SANULI_H
#define GAME_SANULI_H
#include "Globals.h"

int sanuli_winstreak = 0;
String currentAnswer = "";
String grid[6][5];   
int status[6][5];
int currentRow = 0, currentCol = 0;
bool sanuliGameOver = false;

// === BEGIN GENERATED WORDLISTS (tools/gen_sanuli_words.py) ===
// answers: 1927  |  accepted-only: 1401  |  total accepted: 3328
// Regenerate with: python3 tools/gen_sanuli_words.py

// Possible answers (also valid guesses).
const char* const answer_list[] PROGMEM = {
  "AALTO", "AAMEN", "AARRE", "AATTO", "AHDAS", "AHKIO", "AHMIA", "AHNAS", "AHTAA", "AHVEN",
  "AIHIO", "AIKOA", "AINES", "AINOA", "AINUT", "AISTI", "AITIO", "AITTA", "AIVAN", "AIVOT",
  "AJAJA", "AJELU", "AJURI", "AKANA", "ALATI", "ALIAS", "ALIBI", "ALKAA", "ALKIO", "ALLAS",
  "ALPPI", "ALTIS", "ALTTO", "AMMUA", "AMMUS", "AMPUA", "ANKEA", "ANKKA", "ANNOS", "ANODI",
  "ANSIO", "ANTAA", "APAJA", "APILA", "APINA", "APURI", "ARINA", "ARKKI", "ARKKU", "ARMAS",
  "AROMI", "ARPOA", "ARVIO", "ASEMA", "ASKEL", "ASTIA", "ASTMA", "ASTUA", "ATOMI", "AUETA",
  "AUKIO", "AUKKO", "AUTIO", "AVAIN", "AVARA", "AVATA", "AVAUS", "AVOIN", "BAARI", "BAMBU",
  "BANJO", "BASSO", "BENSA", "BIISI", "BINGO", "BONGO", "BOOLI", "BUSSI", "BÄNDI", "CHILI",
  "CURRY", "DIIVA", "DUUNI", "DYYNI", "EDETÄ", "EHTIÄ", "EHTOO", "EHTYÄ", "EILEN", "EINES",
  "EKSYÄ", "ELPYÄ", "ELÄIN", "ELÄMÄ", "ELÄVÄ", "EMPIÄ", "ENNEN", "ENSIN", "EPELI", "ERITE",
  "EROTA", "ESIIN", "ESINE", "ESITE", "ESTYÄ", "ESTÄÄ", "ETANA", "ETELÄ", "ETEVÄ", "ETSIN",
  "ETSIÄ", "EUKKO", "FAKSI", "FAKTA", "FAUNA", "FIKSU", "FILMI", "FINNI", "FIRMA", "FOLIO",
  "FUKSI", "GAALA", "GEENI", "GLÖGI", "HAAMU", "HAAPA", "HAARA", "HAAVA", "HAAVE", "HAAVI",
  "HAHLO", "HAHMO", "HAIKU", "HAIMA", "HAKEA", "HAKKU", "HALKO", "HALLA", "HALLI", "HALPA",
  "HANHI", "HANKE", "HANKI", "HANKO", "HAPAN", "HAPPI", "HAPPO", "HAPSU", "HARHA", "HARJA",
  "HARJU", "HARMI", "HAROA", "HARSO", "HARVA", "HASSU", "HATTU", "HAUDE", "HAUIS", "HAUKI",
  "HAULI", "HAUTA", "HAUVA", "HEHKU", "HEILA", "HEIMO", "HEINÄ", "HELEÄ", "HELLA", "HELLE",
  "HELLÄ", "HELMA", "HELMI", "HEMMO", "HENKI", "HENTO", "HEPPA", "HEPPU", "HERJA", "HERMO",
  "HERNE", "HERRA", "HERUA", "HETKI", "HIDAS", "HIENO", "HIHNA", "HIILI", "HIIRI", "HIISI",
  "HIIVA", "HIKKA", "HILLA", "HILLO", "HILSE", "HINDU", "HINKU", "HINTA", "HIOMO", "HIOTA",
  "HIPOA", "HIPPA", "HIPPI", "HIPPU", "HIRMU", "HIRSI", "HIRVI", "HISSI", "HITSI", "HITTI",
  "HITTO", "HIVEN", "HOHDE", "HOHTO", "HOITO", "HOIVA", "HOKEA", "HOLVI", "HOMMA", "HONKA",
  "HOOPO", "HOPEA", "HOPPU", "HORMI", "HOSUA", "HOURE", "HUILU", "HUIMA", "HUIVI", "HUKKA",
  "HULLU", "HUMMA", "HUMUS", "HUNTU", "HUOLI", "HUONE", "HUONO", "HUOPA", "HUPPU", "HURJA",
  "HURME", "HUULI", "HUUMA", "HUUME", "HUURU", "HUUTO", "HYHMÄ", "HYLJE", "HYLKY", "HYLLY",
  "HYLSY", "HYMIÖ", "HYMNI", "HYPPY", "HYRRÄ", "HYTTI", "HYVIN", "HYÖTY", "HÄIJY", "HÄIVÄ",
  "HÄKKI", "HÄNTÄ", "HÄPEÄ", "HÄRKÄ", "HÄVIÖ", "HÄÄTÖ", "HÄÄVI", "HÄÄYÖ", "HÖHLÄ", "HÖLMÖ",
  "HÖPSÖ", "HÖYLÄ", "HÖYRY", "IDOLI", "IHANA", "IIRIS", "IKINÄ", "IKONI", "IKÄVÄ", "ILKEÄ",
  "ILKIÖ", "ILMAN", "ILMIÖ", "ILVES", "IMELÄ", "IMURI", "ININÄ", "INSSI", "INTRO", "INTTI",
  "IPANA", "ISKEÄ", "ISTUA", "ISYYS", "ITKEÄ", "IVATA", "IÄKSI", "IÄKÄS", "IÄTÖN", "JAHTI",
  "JAKAA", "JAKKU", "JAKSO", "JALAN", "JALAS", "JALKA", "JALUS", "JAOKE", "JARRU", "JATKE",
  "JATKO", "JAUHE", "JEKKU", "JEMMA", "JENGI", "JOHDE", "JOHTO", "JOIKU", "JOKIN", "JOLLA",
  "JOOGA", "JOSPA", "JOTEN", "JOUHI", "JOULU", "JOUSI", "JUHLA", "JUHTA", "JULKI", "JULMA",
  "JUNNU", "JUODA", "JUOMA", "JUOMU", "JUONI", "JUORU", "JUOTE", "JUOVA", "JUPPI", "JUTTU",
  "JUURI", "JYLHÄ", "JYSKE", "JYTKE", "JÄLKI", "JÄNIS", "JÄNNE", "JÄNNÄ", "JÄREÄ", "JÄRIN",
  "JÄRVI", "JÄSEN", "JÄTKÄ", "JÄTTI", "JÄTÖS", "JÄYHÄ", "JÄYNÄ", "JÄÄDÄ", "JÄÄHY", "JÄÄMÄ",
  "JÄÄRÄ", "JÄÄVI", "KAALI", "KAAOS", "KAAPU", "KAARA", "KAARI", "KAASO", "KAASU", "KAATO",
  "KAAVA", "KAHJO", "KAHLE", "KAHVA", "KAHVI", "KAIDE", "KAIHI", "KAIHO", "KAIKU", "KAIMA",
  "KAINO", "KAIVO", "KAKKA", "KAKKU", "KAKSI", "KALJA", "KALJU", "KALLO", "KALMA", "KALPA",
  "KALVO", "KAMMO", "KAMPA", "KAMPI", "KANDI", "KANKI", "KANNE", "KANNU", "KANSA", "KANSI",
  "KANTA", "KANTO", "KAPEA", "KARHU", "KARJA", "KARJU", "KARKU", "KARMA", "KARMI", "KARVA",
  "KASKI", "KASKU", "KASSA", "KASSI", "KASTE", "KASTI", "KASVI", "KASVO", "KASVU", "KATKO",
  "KATKU", "KATOS", "KATSE", "KATTI", "KATTO", "KATUA", "KATVE", "KAUAN", "KAUAS", "KAUHA",
  "KAUHU", "KAUNA", "KAURA", "KAUSI", "KAVIO", "KEBAB", "KEHNO", "KEHTO", "KEHUA", "KEHYS",
  "KEIJU", "KEILA", "KEINU", "KEISO", "KEKSI", "KELJU", "KELLO", "KELMI", "KELPO", "KEMIA",
  "KENKÄ", "KENNO", "KEPEÄ", "KEPPI", "KERHO", "KERIÄ", "KERMA", "KERSA", "KERTA", "KERUU",
  "KESIÄ", "KESTO", "KETJU", "KETKU", "KETTU", "KEULA", "KEVEÄ", "KEVYT", "KEVÄT", "KIELI",
  "KIELO", "KIERO", "KIHTI", "KIILA", "KIIMA", "KIIRE", "KIIVI", "KIKKA", "KILJU", "KILKE",
  "KILPA", "KILPI", "KILSA", "KILTA", "KIMEÄ", "KINGI", "KINOS", "KIPEÄ", "KIPPO", "KIPSI",
  "KIREÄ", "KIRIÄ", "KIRJA", "KIRJE", "KIRNU", "KIRVA", "KISKO", "KISSA", "KITKA", "KITTI",
  "KITUA", "KIUAS", "KIULU", "KIUSA", "KIVES", "KLAPI", "KLUBI", "KOALA", "KOHDE", "KOHTA",
  "KOHTI", "KOHTU", "KOIPI", "KOIRA", "KOITE", "KOIVU", "KOKEA", "KOKKI", "KOKKO", "KOLEA",
  "KOLHU", "KOLME", "KOLVI", "KOMEA", "KOMPA", "KONNA", "KOODI", "KOOMA", "KOOTA", "KOPEA",
  "KOPIO", "KOPLA", "KOPPA", "KOPPI", "KORKO", "KORPI", "KORSI", "KORVA", "KOSIA", "KOSKA",
  "KOSKI", "KOSTO", "KOTKA", "KOULU", "KOURA", "KOURU", "KOVIN", "KOVIS", "KUDIN", "KUDOS",
  "KUHMU", "KUILU", "KUITU", "KUIVA", "KUKIN", "KUKKA", "KUKKO", "KUKSA", "KULHO", "KULKU",
  "KULMA", "KULTA", "KULUA", "KUMEA", "KUMMA", "KUMMI", "KUMPI", "KUMPU", "KUNTA", "KUNTO",
  "KUOHU", "KUOLA", "KUOMU", "KUONA", "KUONO", "KUORI", "KUORO", "KUOSI", "KUOVI", "KUPLA",
  "KUPPI", "KUPRU", "KURJA", "KURKI", "KUROA", "KURRE", "KURVI", "KUSKI", "KUTEA", "KUTEN",
  "KUTOA", "KUTSU", "KUULA", "KUULO", "KUUMA", "KUUME", "KUURA", "KUURI", "KUURO", "KUUSI",
  "KUVIO", "KYETÄ", "KYHMY", "KYLKI", "KYLLÄ", "KYLMÄ", "KYLPY", "KYLVÖ", "KYNIÄ", "KYNSI",
  "KYNTÖ", "KYPSÄ", "KYRPÄ", "KYSYÄ", "KYTEÄ", "KYTKY", "KYTTÄ", "KYYLÄ", "KYYRY", "KYYTI",
  "KÄHEÄ", "KÄNNI", "KÄNSÄ", "KÄREÄ", "KÄRKI", "KÄRRY", "KÄRSÄ", "KÄSKY", "KÄTKÖ", "KÄYDÄ",
  "KÄYPÄ", "KÄYRÄ", "KÄYVÄ", "KÄÄMI", "KÄÄPÄ", "KÄÄRE", "KÄÄRÖ", "KÄÄTY", "KÖHIÄ", "KÖKKÖ",
  "KÖYHÄ", "KÖYSI", "KÖYTE", "LAAJA", "LAAKA", "LAAKI", "LAAMA", "LAARI", "LAATU", "LAAVA",
  "LAAVU", "LAHJA", "LAHJE", "LAHKO", "LAHNA", "LAHTI", "LAIHA", "LAINA", "LAINE", "LAITA",
  "LAITE", "LAIVA", "LAKEA", "LAKKA", "LAKKI", "LAKKO", "LAMPI", "LANKA", "LANKO", "LANNE",
  "LANTA", "LAPIO", "LAPPI", "LAPPU", "LAPSI", "LASKU", "LASSO", "LASTA", "LASTI", "LASTU",
  "LATOA", "LATVA", "LAUDE", "LAUHA", "LAULU", "LAUMA", "LAUSE", "LAUTA", "LAVEA", "LEHMÄ",
  "LEHTI", "LEHTO", "LEHVÄ", "LEIJA", "LEIKE", "LEIMA", "LEIPÄ", "LEIRI", "LEMPI", "LEPPÄ",
  "LEPSU", "LESKI", "LESTI", "LETKA", "LETKU", "LETTI", "LETTU", "LEUKA", "LEUTO", "LEVEÄ",
  "LIATA", "LIEJU", "LIEKA", "LIEMI", "LIERI", "LIERO", "LIESI", "LIETE", "LIEVÄ", "LIHAS",
  "LIHOA", "LIIAN", "LIIGA", "LIIKA", "LIIKE", "LIIMA", "LIINA", "LIITE", "LIITO", "LIITU",
  "LIIVI", "LIKKA", "LIKSA", "LILJA", "LIMSA", "LINJA", "LINKO", "LINNA", "LINTU", "LIOTA",
  "LIPAS", "LIPEÄ", "LIPPA", "LIPPU", "LIPUA", "LIRUA", "LISKO", "LISTA", "LISÄÄ", "LITKU",
  "LITRA", "LIUKU", "LIUOS", "LIUTA", "LOATA", "LOHKO", "LOHTU", "LOIMI", "LOIMU", "LOIVA",
  "LOJUA", "LOKKI", "LOMMO", "LOOSI", "LOOTA", "LOPEN", "LOPPU", "LORDI", "LOSKA", "LOSSI",
  "LOTTO", "LUISU", "LUKEA", "LUKIO", "LUKKI", "LUKKO", "LUMME", "LUNKI", "LUODA", "LUODE",
  "LUOJA", "LUOLA", "LUOMI", "LUONA", "LUOTA", "LUOTI", "LUOTO", "LUOVA", "LUSIA", "LUULO",
  "LUUMU", "LUURI", "LUUTA", "LYHTY", "LYHYT", "LYIJY", "LYKKY", "LYPSY", "LYSEO", "LYSTI",
  "LYYRA", "LYÖDÄ", "LYÖJÄ", "LÄHDE", "LÄHES", "LÄHIN", "LÄHIÖ", "LÄHTÖ", "LÄKSY", "LÄMPÖ",
  "LÄNSI", "LÄPPÄ", "LÄRVI", "LÄSKI", "LÄSNÄ", "LÄTKÄ", "LÄTTY", "LÄÄKE", "LÄÄNI", "LÄÄVÄ",
  "LÖLLÖ", "LÖTKÖ", "LÖYHÄ", "LÖYLY", "LÖYSÄ", "LÖYTÖ", "MAALI", "MAATA", "MAHLA", "MAHTI",
  "MAILA", "MAILI", "MAINE", "MAITO", "MAKEA", "MAKSA", "MAKSU", "MAKUU", "MALJA", "MALLI",
  "MALMI", "MAMMA", "MANGO", "MAPPI", "MARJA", "MARSU", "MASKI", "MASSA", "MASTO", "MATKA",
  "MATSI", "MATTO", "MEDIA", "MEKKO", "MELKO", "MELOA", "MENNÄ", "MERTA", "MESOA", "MESSU",
  "METKA", "METKU", "METRI", "METRO", "METSO", "METSÄ", "MIELI", "MIETE", "MIETO", "MIHIN",
  "MIINA", "MIKKI", "MIKRO", "MIKSI", "MILLI", "MIMMI", "MINIÄ", "MINNE", "MIRRI", "MISSI",
  "MISSÄ", "MISTÄ", "MITEN", "MITTA", "MOITE", "MOKKA", "MONTA", "MOPPI", "MOTTI", "MOTTO",
  "MUHIA", "MUIJA", "MUKSU", "MULTA", "MUMMI", "MUMMO", "MUMMU", "MUNIA", "MUONA", "MUORI",
  "MUOTI", "MUOTO", "MUOVI", "MUREA", "MURHA", "MURHE", "MURJU", "MURSU", "MURTO", "MUSEO",
  "MUSTA", "MUSTE", "MUTKA", "MUTTA", "MUUAN", "MUULI", "MUURI", "MUUSI", "MYKKÄ", "MYLLY",
  "MYSKI", "MYSLI", "MYSSY", "MYTTY", "MYYDÄ", "MYYJÄ", "MYYRÄ", "MYYTY", "MYÖHÄ", "MYÖTÄ",
  "MÄMMI", "MÄNTY", "MÄNTÄ", "MÄRKÄ", "MÄSKI", "MÄTKY", "MÄTÄS", "MÄYRÄ", "MÄÄRE", "MÄÄRÄ",
  "MÖKKI", "MÖLLI", "MÖMMÖ", "MÖNJÄ", "MÖREÄ", "MÖRKÖ", "MÖSSÖ", "NAALI", "NAAMA", "NAAVA",
  "NAHKA", "NAIDA", "NAKKI", "NALLE", "NALLI", "NAPPI", "NARRI", "NASSU", "NASTA", "NATSI",
  "NAUHA", "NAULA", "NAURU", "NAUTA", "NEITI", "NEITO", "NELIÖ", "NELJÄ", "NESTE", "NETTO",
  "NEULA", "NEULE", "NEUVO", "NIDOS", "NIELU", "NIEMI", "NIKSI", "NIPPU", "NIRSO", "NISKA",
  "NIVEL", "NOETA", "NOITA", "NOKKA", "NOLLA", "NOPEA", "NOPPA", "NORSU", "NOSTE", "NOSTO",
  "NOTKO", "NOUSU", "NOUTO", "NUHDE", "NUIJA", "NUIVA", "NUKKA", "NUKKE", "NUMMI", "NUNNA",
  "NUOLI", "NUORA", "NUORI", "NUPPI", "NURJA", "NURMI", "NUTTU", "NUUKA", "NYKIÄ", "NYPPY",
  "NYREÄ", "NYSTY", "NYÖRI", "NÄHDÄ", "NÄKYÄ", "NÄLKÄ", "NÄNNI", "NÄPPI", "NÄPPY", "NÄPSÄ",
  "NÄRHI", "NÄTTI", "NÄYTE", "NÄÄTÄ", "NÖSSÖ", "NÖYRÄ", "OHARI", "OHETA", "OHIMO", "OHJAS",
  "OHJUS", "OIETA", "OIKEA", "OIKKU", "OIKOA", "OINAS", "OMENA", "OMMEL", "ONKIA", "ONTTO",
  "ONTUA", "OPPIA", "OPTIO", "ORAVA", "OSAKE", "OSATA", "OSTAA", "OSTOS", "OSUMA", "OSUUS",
  "OSUVA", "OTTAA", "OVELA", "PAALI", "PAALU", "PAAVI", "PAETA", "PAHKA", "PAHUS", "PAHVI",
  "PAINE", "PAINI", "PAINO", "PAISE", "PAITA", "PAKKA", "PAKKI", "PAKKO", "PAKSU", "PALAA",
  "PALJE", "PALJU", "PALLI", "PALLO", "PALMU", "PALUU", "PANDA", "PANNA", "PANNU", "PANOS",
  "PANTA", "PAPPA", "PAPPI", "PARAS", "PARKU", "PARRU", "PARSA", "PARSI", "PARTA", "PARVI",
  "PASSI", "PASKA", "PASTA", "PATJA", "PATTI", "PAUHU", "PAUKE", "PAUNA", "PEHMO", "PEILI",
  "PEITE", "PELKO", "PELLE", "PELTI", "PELTO", "PENNI", "PENTU", "PEPPU", "PERHE", "PERHO",
  "PERIÄ", "PERSO", "PERSE", "PERUA", "PERUS", "PESIÄ", "PESTI", "PESTÄ", "PESUE", "PETOS",
  "PEURA", "PIANO", "PIDIN", "PIDOT", "PIELI", "PIENI", "PIERU", "PIHKA", "PIHVI", "PIIKA",
  "PIILO", "PIIMÄ", "PIINA", "PIIRI", "PIKKU", "PILKE", "PILLI", "PILLU", "PILVI", "PIMEÄ",
  "PINNA", "PINNI", "PINTA", "PIRTU", "PISKI", "PISSA", "PISTE", "PISTO", "PITKO", "PITKÄ",
  "PITSA", "PITSI", "PITÄÄ", "PIUHA", "PIZZA", "POHJA", "POHJE", "POIJU", "POIKA", "POIMU",
  "POKKA", "POLKU", "POLLA", "POLTE", "POLVI", "POMMI", "PONSI", "POSKI", "POSSU", "POSTI",
  "POTEA", "POTKA", "POTKU", "POTTA", "POTTI", "POTTU", "POUTA", "PUHKI", "PUHTI", "PUHUA",
  "PUIDA", "PUITE", "PUJOA", "PUJOS", "PUKEA", "PUKKI", "PULJU", "PULLA", "PULLO", "PULMA",
  "PUMMI", "PUNOA", "PUNOS", "PUNTA", "PUOLA", "PUOLI", "PUOMI", "PUOTI", "PURJE", "PURJO",
  "PURKU", "PURRA", "PURSI", "PUSKA", "PUSKU", "PUSSI", "PUTKA", "PUTKI", "PUUHA", "PUUMA",
  "PUURO", "PUUTE", "PYLLY", "PYSSY", "PYSTI", "PYSTY", "PYSYÄ", "PYTTY", "PYYHE", "PYÖRÄ",
  "PÄIVÄ", "PÄKIÄ", "PÄSSI", "PÄTEÄ", "PÄTKÄ", "PÄÄSY", "PÄÄTE", "PÄÄTY", "PÖHKÖ", "PÖLJÄ",
  "PÖLLÖ", "PÖRRÖ", "PÖTKÖ", "PÖYTÄ", "RAAJA", "RAAKA", "RAAMI", "RAATI", "RAATO", "RADIO",
  "RAHKA", "RAHTI", "RAIDE", "RAILO", "RAINA", "RAITA", "RAIVO", "RAKAS", "RAKKI", "RAKKO",
  "RALLI", "RAMPA", "RANKA", "RANNE", "RANTA", "RAPEA", "RAPPU", "RAPSI", "RAPSU", "RASIA",
  "RASSU", "RASTA", "RASTI", "RASVA", "RATAS", "RATSU", "RATTI", "RAUHA", "RAUTA", "RAUTU",
  "RAVIT", "REHTI", "REIKÄ", "REILU", "REISI", "REKKA", "REMMI", "RENKI", "RENTO", "REPIÄ",
  "REPPU", "RETEÄ", "RETKI", "RETKU", "REUMA", "REUNA", "RIEHA", "RIEMU", "RIENA", "RIEPU",
  "RIESA", "RIHMA", "RIIHI", "RIIMI", "RIIMU", "RIISI", "RIITA", "RIKAS", "RIKKA", "RIKKI",
  "RIKOS", "RINKI", "RINNE", "RINTA", "RIPEÄ", "RIPPI", "RIPSI", "RISKI", "RISOA", "RISTI",
  "RITSA", "RIUKU", "ROHMU", "ROHTO", "ROIHU", "ROINA", "ROKKI", "ROKKO", "ROMMI", "ROOLI",
  "ROSKA", "ROSTI", "ROSVO", "ROTKO", "ROTSI", "ROTTA", "ROUHE", "ROUTA", "ROUVA", "ROVIO",
  "RUHJE", "RUKKI", "RUKSI", "RULLA", "RUMPU", "RUNKO", "RUOHO", "RUOKA", "RUORI", "RUOTO",
  "RUOTU", "RUSKA", "RUSTO", "RUTTO", "RUTTU", "RUUHI", "RUUMA", "RUUNA", "RUUSU", "RUUTI",
  "RUUTU", "RUUVI", "RYHMÄ", "RYHTI", "RYIJY", "RYKIÄ", "RYPEÄ", "RYPPY", "RYPSI", "RYPÄS",
  "RYSKE", "RYSTY", "RYTKE", "RYTMI", "RYYNI", "RYYSY", "RÄHMÄ", "RÄKIÄ", "RÄNNI", "RÄNTÄ",
  "RÄTTI", "RÄÄSY", "RÄÄVI", "RÖLLI", "RÖNSY", "RÖTÖS", "RÖÖKI", "RÖÖRI", "SAADA", "SAAJA",
  "SAAME", "SAARI", "SAATE", "SAAVI", "SADAS", "SAETA", "SAHTI", "SAITA", "SAKEA", "SAKKA",
  "SAKKI", "SAKKO", "SAKSA", "SALAA", "SALDO", "SALKO", "SALMI", "SALPA", "SAMEA", "SAMPI",
  "SAMPO", "SANKA", "SANKO", "SANOA", "SANTA", "SAOTA", "SAPPI", "SARJA", "SARKA", "SARVI",
  "SATAA", "SAUHU", "SAUMA", "SAUNA", "SAUVA", "SEIMI", "SEINÄ", "SEITI", "SEKSI", "SELKO",
  "SELKÄ", "SELLI", "SELLO", "SELLU", "SELVÄ", "SEOTA", "SEPPÄ", "SERMI", "SETTI", "SEULA",
  "SEURA", "SEUTU", "SIELU", "SIENI", "SIEVÄ", "SIHTI", "SIIKA", "SIILI", "SIILO", "SIIMA",
  "SIINÄ", "SIIPI", "SIITÄ", "SIIVU", "SIKIÖ", "SIKSI", "SILEÄ", "SILLI", "SILLÄ", "SILMU",
  "SILMÄ", "SILTA", "SILTI", "SINKO", "SINNE", "SINUT", "SIPSI", "SISAR", "SISIN", "SISKO",
  "SISSI", "SISUS", "SITEN", "SITOA", "SIVEÄ", "SOHIA", "SOHJO", "SOHVA", "SOIDA", "SOIJA",
  "SOIRO", "SOKEA", "SOKKA", "SOKKO", "SOLKI", "SOLMU", "SONNI", "SONTA", "SOODA", "SOOLO",
  "SOOSI", "SOPIA", "SOPPA", "SOPPI", "SORMI", "SORSA", "SORTO", "SORVI", "SOTIA", "SOTKA",
  "SOTKU", "SOUTU", "SUHDE", "SUJUA", "SUKIA", "SUKKA", "SUKSI", "SULAA", "SULKA", "SULKU",
  "SUMEA", "SUMMA", "SUODA", "SUOJA", "SUOLA", "SUOLI", "SUOMI", "SUOMU", "SUONI", "SUORA",
  "SUPPO", "SURKU", "SURMA", "SURRA", "SUTIA", "SUTTU", "SUURE", "SUURI", "SYDÄN", "SYKSY",
  "SYLKI", "SYLKY", "SYNTI", "SYNTY", "SYRJÄ", "SYSIÄ", "SYYHY", "SYYLÄ", "SYYTE", "SYÖDÄ",
  "SYÖJÄ", "SYÖPÄ", "SÄHKE", "SÄHKÖ", "SÄHLY", "SÄILÄ", "SÄILÖ", "SÄKKI", "SÄLLI", "SÄNKI",
  "SÄNKY", "SÄPPI", "SÄRKI", "SÄRKY", "SÄRMÄ", "SÄVEL", "SÄYNE", "SÄÄLI", "SÄÄRI", "SÄÄTY",
  "SÄÄTÖ", "TAAJA", "TAALA", "TAATA", "TAHKO", "TAHMA", "TAHNA", "TAHRA", "TAHTI", "TAHTO",
  "TAIDE", "TAIKA", "TAIMI", "TAITE", "TAITO", "TAKIA", "TAKKA", "TAKKI", "TAKKU", "TAKOA",
  "TAKSA", "TAKSI", "TAKUU", "TALJA", "TALLI", "TALVI", "TAMMA", "TAMMI", "TANGO", "TANHU",
  "TANKO", "TAPPI", "TAPPO", "TARHA", "TARMO", "TARRA", "TARVE", "TASAN", "TASKU", "TASSU",
  "TATTI", "TAUKO", "TAULU", "TAUTI", "TAVIS", "TEEMA", "TEERI", "TEESI", "TEHDÄ", "TEINI",
  "TEKNO", "TELOA", "TEMPO", "TENHO", "TERHO", "TERMI", "TERVA", "TERVE", "TERÄS", "TESTI",
  "TIEDE", "TIETO", "TIHEÄ", "TIHKU", "TIILI", "TIIMI", "TIINE", "TIIRA", "TIKAS", "TIKKA",
  "TIKKI", "TIKKU", "TILHI", "TILLI", "TILUS", "TIPPA", "TIPPI", "TISKI", "TISLE", "TISSI",
  "TIUHA", "TIUKU", "TOIMI", "TOIVE", "TOIVO", "TOLLO", "TONNI", "TOPPI", "TORNI", "TORUA",
  "TORVI", "TOSIN", "TOSSU", "TOTTA", "TOUHU", "TUHAT", "TUHKA", "TUHMA", "TUHRU", "TUHTI",
  "TUHTO", "TUIKE", "TUIMA", "TUKEA", "TUKKA", "TUKKI", "TUKKO", "TUKKU", "TUKOS", "TULLA",
  "TULLI", "TULOS", "TULVA", "TUMMA", "TUNKU", "TUNNE", "TUNTI", "TUODA", "TUOHI", "TUOJA",
  "TUOLI", "TUOMI", "TUORE", "TUOTE", "TUPLA", "TUPPI", "TUPPO", "TUPSU", "TURHA", "TURMA",
  "TURPA", "TURTA", "TURVA", "TURVE", "TUSKA", "TUSSI", "TUTKA", "TUTTI", "TUTTU", "TUULI",
  "TUUMA", "TUURI", "TYHJÄ", "TYHMÄ", "TYKKI", "TYLSÄ", "TYNKÄ", "TYPPI", "TYRIÄ", "TYRMÄ",
  "TYRNI", "TYTTÖ", "TYTÄR", "TYYLI", "TYYNI", "TYYNY", "TÄHKÄ", "TÄHTI", "TÄHÄN", "TÄKKI",
  "TÄLLI", "TÄNNE", "TÄPLÄ", "TÄSSÄ", "TÄSTÄ", "TÄTEN", "TÄYSI", "TÄYTE", "TÖLLI", "TÖNIÄ",
  "TÖRKY", "TÖRMÄ", "TÖYRY", "UHATA", "UHKEA", "UHKUA", "UHOTA", "UINTI", "UINUA", "UITTO",
  "UJOUS", "ULINA", "ULJAS", "ULKOA", "ULOIN", "ULOKE", "ULVOA", "UNSSI", "UPEUS", "UPOTA",
  "UPOTE", "URHEA", "USEIN", "USKOA", "UTARE", "UTELU", "UUMEN", "UUPUA", "UURAS", "UURNA",
  "UURRE", "UUSIA", "UUTTO", "VAAKA", "VAALI", "VAARA", "VAARI", "VAASI", "VAATE", "VAHTI",
  "VAHVA", "VAIHE", "VAIMO", "VAINO", "VAINU", "VAISU", "VAITI", "VAIVA", "VAJAA", "VAKAA",
  "VAKIO", "VAKKA", "VALAA", "VALAS", "VALHE", "VALIO", "VALJU", "VALLI", "VALTA", "VALUA",
  "VAMMA", "VANHA", "VANKI", "VANNE", "VANUA", "VAPAA", "VAPPU", "VARAS", "VARIS", "VARJO",
  "VARMA", "VAROA", "VARPU", "VARSA", "VARSI", "VASEN", "VASKI", "VASTA", "VASTE", "VATSA",
  "VATTU", "VAUNU", "VAUVA", "VEDOS", "VEHJE", "VEHNÄ", "VEISU", "VEIVI", "VELHO", "VELKA",
  "VELLI", "VELMU", "VENYÄ", "VERBI", "VERHO", "VERSO", "VERTA", "VESSA", "VETÄÄ", "VIDEO",
  "VIEDÄ", "VIEHE", "VIEJÄ", "VIELÄ", "VIENO", "VIHJE", "VIHKO", "VIHTA", "VIILA", "VIILI",
  "VIILU", "VIIMA", "VIIME", "VIINA", "VIINI", "VIIRI", "VIIRU", "VIISI", "VIISU", "VIITE",
  "VIIVA", "VIIVE", "VILJA", "VILLA", "VILLI", "VIMMA", "VINHA", "VINKU", "VIPPI", "VIREÄ",
  "VIRHE", "VIRKA", "VIRKE", "VIRSI", "VIRTA", "VIRUA", "VIRUS", "VISIO", "VISKI", "VISSY",
  "VITJA", "VITSA", "VITSI", "VITTU", "VIULU", "VODKA", "VOIDA", "VOIDE", "VOIMA", "VUODE",
  "VUOHI", "VUOKA", "VUOLU", "VUONO", "VUORI", "VUORO", "VUOSI", "VUOTO", "VYÖRY", "VYÖTE",
  "VÄHIN", "VÄHÄN", "VÄITE", "VÄLJÄ", "VÄSYÄ", "VÄTYS", "VÄYLÄ", "VÄÄRÄ", "YHTIÖ", "YHTYE",
  "YHTYÄ", "YKSIN", "YKSIÖ", "YLPEÄ", "YLTYÄ", "YLTÄÄ", "YLVÄS", "YRMEÄ", "YRTTI", "YSKIÄ",
  "YÖASU", "YÖPUU", "YÖPYÄ", "YÖTYÖ", "YÖTÖN", "YÖUNI", "ÄHINÄ", "ÄHKIÄ", "ÄITYÄ", "ÄKKIÄ",
  "ÄLKÄÄ", "ÄLYTÄ", "ÄMYRI", "ÄPÄRÄ", "ÄREYS", "ÄRINÄ", "ÄRJYÄ", "ÄRTYÄ", "ÄSKEN", "ÄYRÄS",
  "ÄÄLIÖ", "ÄÄNNE", "ÖINEN", "ÖISIN", "ÖLINÄ", "ÖRINÄ", "SAKSI"
};

// Extra accepted guesses that are never the answer.
const char* const extra_list[] PROGMEM = {
  "AALOE", "AARIA", "AARNI", "AASIA", "AATOS", "AATRA", "AHAVA", "AHDIN", "AIDAS", "AIOLI",
  "AIROT", "AIRUE", "AIRUT", "AITOA", "AJAVA", "AKTIO", "ALAVA", "ALETA", "ALIEN", "ALITE",
  "ALOKE", "ALUKE", "ALUNA", "AMBRA", "AMEBA", "AMMUU", "ANELU", "ANIME", "ANOJA", "ANOVA",
  "ANSAS", "APEUS", "APNEA", "APPAA", "APSIS", "ARABI", "ARAVA", "ARENA", "ARGON", "ASANA",
  "ASEMO", "ASETE", "ASKAR", "ASTIN", "ASUJA", "ASUMA", "ASUVA", "ATARI", "ATLAS", "ATRIA",
  "ATULA", "AUDIO", "AUKEA", "AUKOA", "AULIO", "AULIS", "AUSSI", "BAANA", "BALSA", "BAMBI",
  "BANNI", "BARBI", "BARDI", "BASKI", "BAUDI", "BEESI", "BEETA", "BEIBI", "BEIGE", "BENJI",
  "BIDEE", "BILSA", "BIMBO", "BIOMI", "BISSE", "BITTI", "BLINI", "BLOGI", "BLUES", "BOKSI",
  "BONGI", "BONUS", "BOOMI", "BOORI", "BOTTI", "BRAVO", "BROMI", "BUKEE", "BULLA", "BURKA",
  "BURSA", "BUUGI", "BUUMI", "BUURI", "CORGI", "COVID", "DAAMI", "DARRA", "DEBET", "DEEKU",
  "DEKKI", "DELTA", "DENIM", "DEPIS", "DIILI", "DILDO", "DINGO", "DIODI", "DIPPA", "DIPPI",
  "DISCO", "DISKO", "DOGMA", "DOGMI", "DONNA", "DORKA", "DOULA", "DURRA", "DURUM", "DUUMA",
  "DUURI", "EBOLA", "EEDEN", "EEPOS", "EESTI", "EETOS", "EHEYS", "EHKEN", "EIPÄS", "ELELY",
  "ELLEI", "ELÄJÄ", "ELÄKE", "EMALI", "EMOJI", "ENETÄ", "ENSIÖ", "ENTTI", "ENTÄS", "ENTÄÄ",
  "EPÄYS", "ERITÄ", "ESSEE", "ESTIN", "ETEEN", "ETKOT", "ETOVA", "ETUUS", "ETYDI", "ETÄNÄ",
  "EVÄTÄ", "EVÄYS", "FAASI", "FAIJA", "FAKKI", "FARAO", "FARMI", "FARSI", "FAUNI", "FETSI",
  "FIINI", "FILEE", "FOBIA", "FOKKA", "FOKUS", "FONDI", "FORMU", "FORTE", "FUDGE", "FUDIS",
  "FUSKU", "FUTIS", "FUUGA", "FYLLI", "FÄÄRI", "FÖÖNI", "GAMMA", "GANJA", "GASTI", "GEELI",
  "GEKKO", "GENRE", "GEODI", "GETTO", "GIMMA", "GONGI", "GOUDA", "GRADU", "GROGI", "GUANO",
  "GUAVA", "GUIRO", "HAILI", "HAISU", "HAITE", "HALAL", "HALIA", "HALJU", "HALKI", "HALMA",
  "HALME", "HALOO", "HALTI", "HALVA", "HANKA", "HANSA", "HAPRO", "HAPSI", "HARME", "HARRI",
  "HARSU", "HARUS", "HASIS", "HASSI", "HATSI", "HAURA", "HAURE", "HAVAS", "HAVEN", "HEFTA",
  "HEILI", "HEISI", "HEITE", "HEKSA", "HELKE", "HELPI", "HELVE", "HENNA", "HEPPI", "HEREÄ",
  "HETIÖ", "HEVIN", "HIEHO", "HIESU", "HIETA", "HIKKE", "HINDI", "HIOJA", "HIOKE", "HIPAT",
  "HIPIÄ", "HIPPO", "HISSA", "HIUKA", "HOHKA", "HOIDE", "HOKKI", "HOLLI", "HORNA", "HORRE",
  "HORTA", "HOURU", "HOUSU", "HUHTA", "HUILI", "HUISI", "HUNNI", "HUOKU", "HUOMA", "HUORA",
  "HUOVI", "HUPSU", "HURMA", "HUSKY", "HUTSU", "HUTTU", "HUUSI", "HUUTI", "HUUVA", "HYTKE",
  "HYYDE", "HYYTÖ", "HYÖKY", "HYÖTÖ", "HÄIVE", "HÄJYT", "HÄLLÄ", "HÄPPÄ", "HÄRME", "HÄRMÄ",
  "HÖLLÄ", "HÖNKÄ", "HÖRHÖ", "HÖSKÄ", "HÖTTÖ", "HÖYDE", "HÖYNÄ", "HÖYTY", "IIBIS", "IIKKA",
  "IKIYÖ", "IKÄNÄ", "IKÄYS", "IKÄÄN", "ILTAA", "ILTTI", "IMAGO", "IMEVÄ", "IMMYT", "IMUKE",
  "INTIA", "INTOS", "INTRA", "IRVIÄ", "ISKOS", "ISLAM", "ISOTA", "ISOUS", "ITARA", "ITÄVÄ",
  "IVAUS", "JAALA", "JAHKA", "JAKKI", "JALLU", "JAMBI", "JAMIT", "JANNU", "JATKA", "JATSI",
  "JAUHO", "JEERA", "JENGA", "JENKA", "JERMU", "JETTI", "JIIRI", "JIPPO", "JOLMA", "JONKA",
  "JONNE", "JOOGI", "JOOLI", "JOPAS", "JOTOS", "JOTTA", "JOULE", "JUKRA", "JULLI", "JUMBO",
  "JUMPE", "JUOJA", "JUOLA", "JUOPA", "JUROA", "JURRI", "JUSSI", "JUTAA", "JYTKY", "JYTÄÄ",
  "JÄKKI", "JÄLSI", "JÄNKÄ", "JÄRKI", "JÄSTI", "JÄTTÖ", "JÖÖTI", "KAADE", "KAANI", "KAHJU",
  "KAIRA", "KAISA", "KAITA", "KAIVU", "KAKOA", "KAKRU", "KALHU", "KALKE", "KALLA", "KALMO",
  "KALSA", "KALVI", "KAMEE", "KAMMI", "KAMPE", "KANJI", "KANNA", "KAPIO", "KAPPA", "KAPPI",
  "KAPSE", "KAPUT", "KARHE", "KARHI", "KARHO", "KARRI", "KARSI", "KARVE", "KARVI", "KASKO",
  "KATKA", "KATKI", "KAUKO", "KAULA", "KAUNO", "KAZOO", "KEESI", "KEHIÄ", "KEHNÄ", "KEHRÄ",
  "KEIJA", "KEINO", "KEITE", "KEKRI", "KELJA", "KELMU", "KELSI", "KELTA", "KENDO", "KEOTA",
  "KERMI", "KERNI", "KERTO", "KESKI", "KESSU", "KESTI", "KETKÄ", "KETSI", "KETTO", "KHAKI",
  "KIDUS", "KIEKU", "KIERÄ", "KIESI", "KIHLA", "KIILU", "KIINA", "KIIRU", "KIISU", "KIITO",
  "KIMMA", "KIMPI", "KIPPA", "KIPPI", "KIRJO", "KIRSI", "KIRSU", "KISKA", "KISMA", "KISSE",
  "KITKU", "KIURU", "KOBRA", "KOETE", "KOHJU", "KOHME", "KOHVA", "KOIJA", "KOISA", "KOISO",
  "KOKKA", "KOKSI", "KOLHO", "KOLJA", "KOLKE", "KOLLI", "KOLLO", "KOLOA", "KONGI", "KONTU",
  "KOPRA", "KOPSE", "KOPSU", "KOPTI", "KOREA", "KORIS", "KORNI", "KORSU", "KORTE", "KORVO",
  "KOSLA", "KOSSU", "KOSTE", "KOTOA", "KOTSA", "KOTTI", "KOTVA", "KOUHO", "KOUSA", "KRAKA",
  "KRAPU", "KRIMI", "KROMI", "KRÄNÄ", "KUIRI", "KUIRU", "KUKKU", "KUKRI", "KULJU", "KULLI",
  "KUNDI", "KUNNE", "KUNPA", "KUOHA", "KUOLO", "KUOMA", "KUORE", "KUPPA", "KUPRO", "KURDI",
  "KURHO", "KURKO", "KURRA", "KURRI", "KUSTA", "KUSTU", "KUTKA", "KUTRI", "KUTTU", "KUUBA",
  "KUULU", "KUVAS", "KYMRI", "KYRSÄ", "KYSSÄ", "KYSTA", "KYSTÄ", "KYÖMY", "KÄKRI", "KÄLLI",
  "KÄMMI", "KÄNNY", "KÄRHI", "KÄRHÖ", "KÄRRI", "KÄYTE", "KÖKSÄ", "KÖLLI", "KÖRMY", "KÖSSI",
  "KÖYRI", "KÖYRY", "KÖYRÄ", "KÖÖRI", "LADOS", "LAFKA", "LAIHO", "LAKSA", "LAMEE", "LANDE",
  "LANIT", "LAOTA", "LAPPO", "LARVA", "LASER", "LASKO", "LASSI", "LATTA", "LATTE", "LEFFA",
  "LEIDI", "LEILI", "LEIMU", "LEINI", "LEIVO", "LELLI", "LEMMA", "LEMPO", "LENKO", "LENTO",
  "LEPRA", "LESBO", "LESTY", "LESTÄ", "LETTO", "LEUKU", "LIEKO", "LIESU", "LIEVE", "LIFTI",
  "LIHIS", "LIILA", "LIIRA", "LIKIN", "LIMBO", "LIPOA", "LIPPI", "LIPPO", "LIRIÄ", "LITKA",
  "LITOA", "LITSA", "LITSI", "LITTA", "LODEN", "LOHJE", "LOKSE", "LOKUM", "LORUA", "LOTJA",
  "LOTTA", "LOUHE", "LOUHI", "LUHKA", "LUHTA", "LUHTI", "LUIHU", "LUIKU", "LUIMU", "LUIRO",
  "LUIRU", "LUJAA", "LUKSI", "LUMEN", "LUNNI", "LUOKO", "LUOMA", "LUOMU", "LUORI", "LUOTU",
  "LUOVI", "LUPPO", "LUSMU", "LUSSU", "LUSTE", "LUSTO", "LUTKA", "LUTKU", "LYHDE", "LYMFA",
  "LYTTY", "LYYDI", "LYYLI", "LYYTI", "LYÖMÄ", "LYÖTY", "LYÖVÄ", "LÄIKE", "LÄKKI", "LÄLLY",
  "LÄNKI", "LÄPSE", "LÄPSY", "LÄSSY", "LÄTSÄ", "LÄTTI", "LÄTTÄ", "LÄÄTE", "LÖSSI", "MAAGI",
  "MAARU", "MAATE", "MACHO", "MAFIA", "MAGIA", "MAGMA", "MAGNA", "MAIJA", "MAITI", "MAIVA",
  "MAKKI", "MAKRO", "MAKSI", "MALKA", "MALTA", "MALTO", "MALVA", "MAMBA", "MANGA", "MANIA",
  "MANNA", "MANSI", "MANTO", "MANTU", "MAORI", "MARTO", "MASSI", "MASSU", "MATAA", "MATTA",
  "MATTI", "MAURI", "MEEMI", "MEIKÄ", "MEILI", "MEKKA", "MELLI", "MELTO", "MENSA", "MENYY",
  "MEPPI", "MERSU", "MESSI", "MESTA", "MIERO", "MIILU", "MIKIN", "MINKÄ", "MIRHA", "MITRA",
  "MODUS", "MOIKE", "MOLLI", "MONES", "MONNI", "MONTE", "MOODI", "MOOLI", "MOPSI", "MORSE",
  "MOSKA", "MOURU", "MUHEA", "MUHJU", "MUHKU", "MUHVI", "MULLI", "MUREN", "MURRE", "MUSSU",
  "MUSTI", "MUTRU", "MUTSI", "MUUMI", "MUUSA", "MUUTA", "MYHKY", "MYKIÖ", "MYONI", "MYRHA",
  "MYYVÄ", "MÄHKÄ", "MÄIHÄ", "MÄIKE", "MÄLLI", "MÄLSÄ", "MÄSSY", "MÄTTÖ", "MÄÄLY", "MÖHKÖ",
  "MÖHNÄ", "MÖLJÄ", "MÖRRI", "MÖTKÖ", "MÖTTI", "MÖYHY", "NAARA", "NACHO", "NAFTA", "NAFTI",
  "NAHAS", "NAITU", "NAIVI", "NAKSU", "NANNA", "NAPPA", "NAPPO", "NAPSU", "NASSE", "NATSA",
  "NEKKA", "NEKKU", "NEPPI", "NETTI", "NIHTI", "NIINI", "NIISI", "NIMIÖ", "NINJA", "NIOBI",
  "NIPPA", "NIRRI", "NISSE", "NISSI", "NISTI", "NITOA", "NITRO", "NIUHO", "NIVOA", "NIVUS",
  "NOONI", "NOPSA", "NORJA", "NORMI", "NORRI", "NORUA", "NUKKI", "NULJU", "NUOLU", "NUPPU",
  "NURIN", "NYHDE", "NYHTÖ", "NYKYÄ", "NYLKY", "NYLON", "NYMFI", "NYNNY", "NYSVÄ", "NÄKKI",
  "NÄREÄ", "NÄRPI", "NÄSIÄ", "ODOTE", "OHEEN", "OHUUS", "OITIS", "OJATA", "OJUKE", "OKAPI",
  "OKSIA", "OLAKE", "OLEVA", "OLIJA", "OMATA", "OMEGA", "OMPPU", "ONNEA", "ORIGO", "ORKKU",
  "OTAVA", "PAANU", "PAARI", "PAASI", "PADEL", "PAHIS", "PAHKI", "PAHNA", "PAKTI", "PALEO",
  "PALHO", "PALJO", "PALKO", "PALLE", "PALOA", "PALSA", "PALTE", "PALVI", "PAMPA", "PANKO",
  "PARKA", "PASHA", "PASKE", "PASMA", "PATEE", "PATIO", "PAULA", "PEELO", "PEESI", "PEFFA",
  "PEHKO", "PEHKU", "PEHVA", "PEKKA", "PELUU", "PENIS", "PENNE", "PEONI", "PERIN", "PERNA",
  "PERSU", "PERVO", "PESIN", "PESIS", "PESKI", "PESTO", "PESTY", "PETSI", "PETTU", "PEUHU",
  "PIENA", "PIETI", "PIETÄ", "PIHIÄ", "PIHTA", "PIHTI", "PIILU", "PIIRU", "PIISI", "PIKAA",
  "PIKEE", "PIMIÖ", "PINJA", "PINKO", "PINNE", "PIONI", "PIRTA", "PISSI", "PLARI", "PLUTO",
  "PLÄSI", "PLÖRÖ", "POIES", "POLIO", "POLJE", "POLLE", "POMSI", "PONDI", "PONNE", "POOKI",
  "POOLI", "POOLO", "POPPA", "POPPI", "PORHO", "PORNO", "POSSE", "POTRA", "PRIKI", "PROTO",
  "PRUJU", "PUDAS", "PUFFI", "PUHDE", "PUHVI", "PUIJA", "PUITU", "PUIVA", "PUKLU", "PULJA",
  "PULMU", "PUNKA", "PUOMU", "PUOSU", "PUPPU", "PURSE", "PURSO", "PURSU", "PUSIA", "PUSLA",
  "PUSTA", "PUTSI", "PUTTI", "PUTTO", "PYGMI", "PYKIÄ", "PYREE", "PYRKY", "PYSTÖ", "PYTON",
  "PYYDE", "PYÖRY", "PYÖRÖ", "PÄHEÄ", "PÄLLI", "PÄLVI", "PÄNNÄ", "PÄTSI", "PÄÄRI", "PÖHNÄ",
  "PÖKKÖ", "PÖKÄT", "PÖLHÖ", "PÖLLI", "PÖLLY", "PÖNKÄ", "PÖTKY", "PÖTSI", "PÖTYÄ", "RAANA",
  "RAANI", "RAANU", "RAAPE", "RAASU", "RAATE", "RABBI", "RADON", "RAFLA", "RAHJE", "RAHNA",
  "RAHTU", "RAISU", "RAITO", "RAKIA", "RAKKA", "RAKSA", "RAKSI", "RAKSU", "RAMBO", "RAMEN",
  "RANKI", "RANKO", "RANTU", "RAPSE", "RASKI", "RASKO", "RASPI", "RASSI", "RATKI", "RATTO",
  "RAUMA", "REIKI", "REIMA", "REIVI", "REKKI", "REKKO", "REKRY", "REKSI", "RESPA", "RESSU",
  "RETEE", "RETRO", "REVYY", "RIEVÄ", "RIHLA", "RIISA", "RIITE", "RIIUU", "RIKKO", "RIKSA",
  "RIKSI", "RIMPI", "RIMSU", "RIPPU", "RIPSU", "RISKU", "RISLA", "RISSA", "RITSI", "RITVA",
  "RODEO", "ROHDE", "ROHJO", "ROILO", "ROIMA", "ROIPE", "ROISI", "ROKKA", "ROMPE", "RONDO",
  "ROOSA", "ROPSE", "ROSEE", "RUGBY", "RUKKA", "RUMBA", "RUNDI", "RUNTU", "RUODE", "RUOJA",
  "RUOKO", "RUONA", "RUOTI", "RUPIA", "RUPLA", "RUPSU", "RUSKO", "RUTKA", "RYHMY", "RYNTÖ",
  "RYTKY", "RYTTY", "RYVÄS", "RYYTI", "RYÖNÄ", "RÄHJÄ", "RÄIKE", "RÄIME", "RÄKKI", "RÄKKÄ",
  "RÄMEÄ", "RÄPPI", "RÄPSE", "RÄSTI", "RÄYHÄ", "RÖIJY", "RÖMEÄ", "RÖSSY", "RÖSTI", "RÖYHY",
  "SAAGA", "SAAGO", "SAALI", "SAAMI", "SAATI", "SAAVA", "SADIN", "SAFKA", "SAHUU", "SAIVO",
  "SALON", "SALSA", "SALVA", "SAMBA", "SANUE", "SATSI", "SAUDI", "SAURO", "SAVES", "SCIFI",
  "SEEBU", "SEETI", "SEILI", "SEIPI", "SEITA", "SEKKA", "SEKKI", "SEKTI", "SELIN", "SELJA",
  "SELLA", "SELUS", "SELYS", "SEPPO", "SERVO", "SETRI", "SIDOS", "SIERA", "SIETO", "SIETÄ",
  "SIGMA", "SIIRA", "SIIVO", "SIKEÄ", "SIKHI", "SIKLI", "SILAT", "SILKO", "SILLA", "SILSA",
  "SIPPI", "SIRRI", "SISAL", "SITAR", "SITKO", "SITRA", "SITSI", "SIULA", "SKEET", "SKENE",
  "SKIDI", "SKINI", "SKOOL", "SNADI", "SNOBI", "SOFTA", "SOIVA", "SOKKI", "SOLUA", "SOMPA",
  "SONDI", "SOOMA", "SOONI", "SOOPA", "SOREA", "SORJA", "SORVA", "SOSSU", "SOTTA", "SOUVI",
  "SPORA", "SPRII", "STADI", "STIDI", "STOUT", "SUDIN", "SUIRO", "SUJUT", "SULHO", "SULJE",
  "SUNNA", "SUNNI", "SUOPA", "SUOVA", "SUPER", "SUPPA", "SURVE", "SUSHI", "SUSSU", "SUTKI",
  "SUULA", "SUULI", "SUURA", "SUUTE", "SYKLI", "SYLTÄ", "SYLYS", "SYYNI", "SYÖTE", "SYÖTY",
  "SÄILE", "SÄKIÄ", "SÄLPÄ", "SÄTKY", "SÄTKÄ", "SÄÄDE", "SÖSSÖ", "TAARA", "TAFTI", "TAHVO",
  "TAIGA", "TAIJI", "TAIVE", "TAIVO", "TAKAA", "TALAS", "TALLA", "TANKA", "TAPAS", "TAPSI",
  "TARKE", "TAROT", "TASSI", "TATAR", "TAULA", "TEHTY", "TEILI", "TELEX", "TELJE", "TELJO",
  "TELKI", "TEMMI", "TERIÖ", "TERNI", "TESLA", "TESMA", "TETRA", "TIARA", "TIERA", "TIIMA",
  "TIINU", "TIKLI", "TILDE", "TILKE", "TILSA", "TIMMI", "TINKA", "TINKI", "TIRRI", "TIRSA",
  "TITTI", "TOETA", "TOKKA", "TOKKO", "TONKI", "TONUS", "TOOGA", "TOOPE", "TOORA", "TOOSA",
  "TOPPA", "TORKE", "TORSO", "TOSIO", "TOSKA", "TOUKO", "TOUVI", "TUHNU", "TUIJA", "TUIKI",
  "TULUS", "TUMMU", "TUNTO", "TUNTU", "TUONI", "TUORA", "TUOVA", "TUPAS", "TUPEE", "TURBO",
  "TURKU", "TURRI", "TURSO", "TUSSU", "TUTIA", "TUTOR", "TUUBA", "TUUBI", "TUURA", "TWEED",
  "TYHJÖ", "TYKKY", "TYLLA", "TYLLI", "TYPÄS", "TYTSY", "TYTTI", "TYVEN", "TÄHDE", "TÄPPÄ",
  "TÄSMÄ", "TÖHKÄ", "TÖHNÄ", "TÖHRY", "TÖLLÖ", "TÖTSÄ", "TÖYRÄ", "UIKKU", "UISKO", "UKEMI",
  "UKULI", "ULOTA", "UMAMI", "UMBRA", "UMPIO", "URETA", "URINA", "USEUS", "UURTO", "UUSIO",
  "UUTOS", "UUTTU", "VAADE", "VAAJA", "VAAMI", "VAATA", "VAKSI", "VALIN", "VALOS", "VALVE",
  "VANJA", "VANNA", "VARHO", "VARPA", "VARTE", "VARUS", "VARVI", "VASOA", "VATJA", "VATTI",
  "VEDIN", "VEHKA", "VEKKI", "VENHE", "VENHO", "VENTO", "VENUS", "VEPSÄ", "VERES", "VERKA",
  "VERME", "VERTO", "VESKA", "VESKI", "VESOA", "VICHY", "VIERI", "VIHMA", "VIHNE", "VIINE",
  "VIIRA", "VIITA", "VIKLO", "VILKE", "VIMPA", "VINKA", "VIOLA", "VIPPA", "VIRIÖ", "VIRMA",
  "VIRNA", "VIRNE", "VIRPI", "VIRSU", "VISPI", "VISSI", "VISTI", "VISVA", "VITKA", "VITOA",
  "VOHLA", "VOIPA", "VOKKI", "VORMU", "VOTKA", "VOUTI", "VULVA", "VUONA", "VUOTA", "VÄIVE",
  "VÄLKE", "VÄLLY", "VÄLYS", "VÄNKÄ", "VÄRVE", "VÄSKY", "WATTI", "WEBBI", "WOKKI", "YHDES",
  "YHDYS", "YHÄTI", "YLETÄ", "YLEVÄ", "YLITE", "YLÄVÄ", "YNINÄ", "YNSEÄ", "YRITE", "YSKÖS",
  "YSÄRI", "YÖAIE", "YÖAJO", "YÖKKÖ", "YÖTEE", "ZOMBI", "ZOOMI", "ÄHKYÄ", "ÄISKÄ", "ÄKEYS",
  "ÄKÄMÄ", "ÄKÄTÄ", "ÄLINÄ", "ÄNKEÄ", "ÄPÄRE", "ÄÄNES", "ÄÄNTÖ", "ÖHINÄ", "ÖLPPÄ", "ÖRKKI",
  "ÖVERI"
};
// === END GENERATED WORDLISTS === 

String getLetter(String word, int index) {
  int logical_idx = 0; int i = 0;
  while(i < word.length()) {
    int char_len = ((word[i] & 0xE0) == 0xC0) ? 2 : 1;
    if (logical_idx == index) return word.substring(i, i + char_len);
    i += char_len; logical_idx++;
  }
  return "";
}

// True if 'guess' (UTF-8) is present in a PROGMEM word array.
bool wordInList(const char* const list[], int count, const char* guess) {
  for (int i = 0; i < count; i++) {
    if (strcmp_P(guess, (char*)pgm_read_ptr(&(list[i]))) == 0) return true;
  }
  return false;
}

bool isValidWord(String guess) {
  int numAnswers = sizeof(answer_list) / sizeof(answer_list[0]);
  int numExtra   = sizeof(extra_list) / sizeof(extra_list[0]);
  return wordInList(answer_list, numAnswers, guess.c_str())
      || wordInList(extra_list,  numExtra,   guess.c_str());
}

void drawSanuliBoard() {
  int startX = 12; int startY = 2; 
  for (int r = 0; r < 6; r++) {
    for (int c = 0; c < 5; c++) {
      int x = startX + c * 21; int y = startY + r * 21;
      uint16_t tileColor = (status[r][c] == 1) ? COLOR_GRAY : (status[r][c] == 2) ? COLOR_YELLOW : (status[r][c] == 3) ? COLOR_GREEN : COLOR_BG;
      tft.fillRect(x, y, 19, 19, tileColor);
      if (tileColor == COLOR_BG) tft.drawRect(x, y, 19, 19, COLOR_BORDER);
      if (grid[r][c] != "") { u8g2_display.setForegroundColor(COLOR_TEXT); u8g2_display.setCursor(x + 3, y + 15); u8g2_display.print(grid[r][c]); }
    }
  }
}

void startNewSanuliGame(); 

void endSanuliGameDisplay(bool won) {
  sanuliGameOver = true; delay(1500); tft.fillScreen(COLOR_BG);
  u8g2_display.setCursor(10, 40);
  if (won) { u8g2_display.setForegroundColor(COLOR_GREEN); u8g2_display.print("VOITTO!"); }
  else { u8g2_display.setForegroundColor(COLOR_GRAY); u8g2_display.print("HÄVISIT :("); u8g2_display.setCursor(10, 70); u8g2_display.print("Sana: " + currentAnswer); }
  u8g2_display.setCursor(10, 105); u8g2_display.setForegroundColor(COLOR_TEXT);
  u8g2_display.print("Putki: "); u8g2_display.print(sanuli_winstreak);
  delay(3000); startNewSanuliGame();
}

void processSanuliGuess() {
  String guess = ""; for(int i=0; i<5; i++) guess += grid[currentRow][i];
  if (!isValidWord(guess)) {
    u8g2_display.setCursor(5, 125); u8g2_display.setForegroundColor(COLOR_YELLOW); u8g2_display.print("EI LISTALLA");
    delay(800); tft.fillRect(0, 110, 128, 18, COLOR_BG); return;
  }
  bool answerUsed[5] = {false, false, false, false, false}; int tempStatus[5] = {1, 1, 1, 1, 1};
  for (int i = 0; i < 5; i++) if (grid[currentRow][i] == getLetter(currentAnswer, i)) { tempStatus[i] = 3; answerUsed[i] = true; }
  for (int i = 0; i < 5; i++) {
    if (tempStatus[i] == 3) continue;
    for (int j = 0; j < 5; j++) if (!answerUsed[j] && grid[currentRow][i] == getLetter(currentAnswer, j)) { tempStatus[i] = 2; answerUsed[j] = true; break; }
  }
  for (int i = 0; i < 5; i++) status[currentRow][i] = tempStatus[i];
  drawSanuliBoard();
  bool won = true; for (int i = 0; i < 5; i++) if (status[currentRow][i] != 3) won = false;
  Preferences prefs; prefs.begin("sanuli", false);
  if (won) { sanuli_winstreak++; prefs.putInt("streak", sanuli_winstreak); endSanuliGameDisplay(true); }
  else if (currentRow == 5) { sanuli_winstreak = 0; prefs.putInt("streak", sanuli_winstreak); endSanuliGameDisplay(false); }
  else { currentRow++; currentCol = 0; }
  prefs.end();
}

void startNewSanuliGame() {
  int numAnswers = sizeof(answer_list) / sizeof(answer_list[0]);
  char buffer[16]; strcpy_P(buffer, (char*)pgm_read_ptr(&(answer_list[random(0, numAnswers)])));
  currentAnswer = String(buffer); currentRow = 0; currentCol = 0; sanuliGameOver = false;
  for (int r = 0; r < 6; r++) for (int c = 0; c < 5; c++) { grid[r][c] = ""; status[r][c] = 0; }
  tft.fillScreen(COLOR_BG); drawSanuliBoard();
}

void initSanuli() {
  Preferences prefs; prefs.begin("sanuli", true); sanuli_winstreak = prefs.getInt("streak", 0); prefs.end();
}

void handleSanuliInput(char c) {
  if (sanuliGameOver) return;
  if (c == 0x08 || c == 0x7F) { if (currentCol > 0) { currentCol--; grid[currentRow][currentCol] = ""; drawSanuliBoard(); } } 
  else if (c == 0x0D || c == 0x0A) { if (currentCol == 5) processSanuliGuess(); } 
  else if (currentCol < 5) { 
    String inputStr = "";
    if (c == '.') inputStr = "Ä"; else if (c == ',') inputStr = "Ö";
    else if (c >= 'a' && c <= 'z') inputStr = String((char)(c - 32));
    else if (c >= 'A' && c <= 'Z') inputStr = String(c);
    if (inputStr != "") { grid[currentRow][currentCol] = inputStr; currentCol++; drawSanuliBoard(); }
  }
}
#endif