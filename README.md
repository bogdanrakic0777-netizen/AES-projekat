# AES implementacija i FUSE

Implementacija AES standarda (*Advanced Encryption Standard*) od nule u jeziku *C*, sa *x86 assembly* optimizacijom i paralelizacijom pomoću niti. Projekat povezuje gradivo iz više predmeta: operativni sistemi, arhitektura računara, algebra, programski jezici i strukture podataka.

## Sadržaj projekta:
Projekat obuhvata potpunu implementaciju AES-a bez korišćenja gotovih kriptografskih biblioteka, uključujući matematičku osnovu u Galoovom polju (konačnom polju) GF(2^8), sve četiri *round* operacije, *Rijndael key schedule* za obe veličine ključa (128 i 256 bita) i dva režima rada.
Podržane su dve veličine ključa: 128 i 256 bita i dva režima rada: *ECB* i *CBC*, i dve varijante izvršavanja *ECB* (sekvencijalno i sa nitima)

## Komponente:

### GF(2^8) aritmetika:

Osnova je aritmetika u ograničenom polju sa 256 elemenata. Sabiranje je implementirano kao *XOR* (sabiranje polinoma po modulu 2 bez prenosa). Množenje se svodi na operaciju *xtime* (u matematičkom smislu množenje polinoma sa X, uz deljenje sa nesvodljivim polinomom ukoliko nakon množenja rezultat ispadne iz polja). 
Inverz se računa preko **male Fermaove teoreme**:
```
a^-1 = a^(p-2) (mod p)
a^-1 = a^(254)  //Primenjeno na GF(2^8)
```
Računanje inverza je implementirano preko **male Fermaove teoreme** ali i preko hardkodovane *lookup* tabele zbog efikasnosti.

### SBOX:

**S-BOX** unosi nelinearnosti u algoritam i svodi se na dva koraka: *inverz* u GF(2^8) uz primenu *Afine transformacije*. Afina transformacija je efektivno množenje Matricom i translacija za vektor. Zbog cikličnosti matrice koja se koristi u AES-u, Afina transformacija je implementirana preko cikličnih rotiranja bitova.

### ROUND OPERACIJE:

Implementirane su četiri operacije koje se primenjuju u svakoj rundi - *SubBytes*, *ShiftRows*, *MixColumns*, *AddRoundKey*, kao i njihove inverzne operacije. *MixColumns* je množenje matricom u GF(2^8) i dodatno je implementirana u *x86 at&t asembleru*. Zbog nekompatibilnosti 32-bitnog asemblera i 64-bitnog fajl sistema, mixcolumns_asm.S se ne koristi u kodu.

### Key schedule
*Rijndael key schedule* generiše ključeve spram originalnog ključa. Za *AES-128* generiše **11**, a za *AES-256* **15** ključeva. Key schedule za *AES-256* je nešto kompleksniji nego za *AES-128*. 

### Modovi rada

*ECB (Electronic Code Book)* enkriptuje svaki blok nezavisno, što ga čini pogodnijim za paralelizaciju, ali je kriptografski slabiji (poznat primer je *AES ECB penguin*). *CBC (Cipher Block Chaining)* povezuje blokove tako što se svaki blok pre enkripcije prvo *XOR-uje* sa prethodnim enkriptovanim blokom, za prvi blok se koristi *inicijalni vektor*. *ECB* je dostupan u sekvencijalnom i paralelnom režimu, a *CBC* samo u sekvencijalnom.

### Paralelizacija

Paralelna *ECB* varijanta deli ulazne blokove na regione i svaka nit nezavisno obradjuje svoj region. Implementacija niti je odradjena u C++. Povezivanje C i C++ koda odrađeno pomoću *extern "C"* mehanizma.

### Kompajliranje

Za kompajliranje su potrebni gcc, g++ i FUSE razvojna biblioteka.
Komanda za kompajliranje svih komponenti: `make`.
Ovim se grade tri izvrsna fajla: aes_fs (FUSE fajl sistem), test_basic (testovi AES jezgra) i test_modes (testovi rezima rada niti).
Komande objasnjene:
```
make aes_fs         # samo FUSE fajl sistem
make test_basic     # testovi AES jezgra
make test_modes     # testira paralelni rezim rada
make demo_basic     # kompajlira i pokrece konzolnu demonstraciju svih AES varijanti
make demo           # komapjlira i pokrece demonstraciju kroz FUSE, enkriptuje ovaj README fajl i daje dekriptovanu verziju za korisnika
make enddemo        # brise dekriptovan fajl i prekida rad FUSE 
make clean          # brise izvrsne fajlove
```
Bitno je na kraju demonstracije pokrenuti komandu make enddemo. Za demonstraciju se koristi ovaj README fajl :) 

### Testiranje

Testovi verifikuju ispravnost prema zvaničnim *FIPS 197 test vektorima* za obe implementirane veličine ključa, kao i ispravnost *roundtrip-ova* (enkripcija pa dekripcija bi trebalo da daje original) i da se varijanta sa nitima ponaša isto kao sekvencijalna.
Testovi se pokreću preko komande 
`make test`.

### FUSE

Pomocu FUSE (Filesystem in userspace), implementiran je jednostavan fajlsistem u kom se za enkriptovanje podataka na disku, i dekriptovanje podataka sa diska koristi implementirani *AES CBC 256* algoritam. 
Komande za mount-ovanje i unmount-ovanje fajl sistema:
```make mount``` i ```make umount``` 
(ako se pokrenu komande za demonstraciju, ovo se desi automatski ispod haube)
Inicijalni vektor i 256-bitni kljuc su hardkodovani u fajlu aes_fuse.c

### Korišćene tehnologije:
Projekat je pisan u *C*-u i *C++*-u, sa delom u *x86 AT&T asembleru*. Za paralelizaciju se koriste *POSIX niti* odnosno *C++ std::thread*. Razvijeno i testirano na *Ubuntu 20.04* sistemu. zeropadding...