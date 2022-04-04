# AVXAssignment
Assignment2 Quicksort mit AVX

### Funktion
Sobald ein QuickSort Objekt erstellt wird, erzeugt der Defaultkonstruktor eine Permutationstabelle. Diese ist 256 Indexe gross und in jedem Index sind Arrays der Grösse 8 abgespeichert. Da diese auch statisch sein kann und beim wiederverwenden vom gleichen Objekt die Tabelle auch neu erzeugt werden würde, habe ich sie in den Konstruktor gestellt und nicht in der Methode selbst.

Beim Start des AVX Sortieralgorithmus wird erst geschaut wie lange die momentane Partition ist(Am Anfang natürlich so gross wie das Datenarray selbst). Falls die Partition keine Länge hat wird die Methode avxSort unterbrochen und returnt.

Falls es 1 bis und mit 16 Elemente in der Partition hat, wird ein Insertionsort angewendet.  Aus Performancegründen macht es sinn, hier nicht teure Rekursionen für wenige Elemente in einer Partition durchzuführen. Das habe ich so aus dem Blogpost https://bits.houmus.org/2020-01-30/this-goes-to-eleven-pt3 übernommen.

Falls es mehr als 16 Elemente in der Partition hat, wird ein der AVX Teil gestartet. Es teilt die jetztige Partition in 2 Partitionen und ruft auf den 2 Partitionen sich selbst rekursiv aus. Das wird so lange ausgeführt bis jede Partition kleiner als 17 wird und vom Insertionsort zuende sortiert wird.

avxPartition lädt ein 256 Bit Register aus der momentanen Partition. Wählt das Pivotelement aus, in dem er das mittlere Element nimmt und vergleicht welche Werte im 256 Bit register grösser sind als das Pivot element. Da bei mir das Resultat nicht in ein 8 bit integer sonder nur in ein 32 bit musste ich einen das 32 bit auf ein 8 Bit integer projezieren. Somit kann ich auch Bedenkenlos die Maske als Index für die 256 grosse Permutationetabelle verwenden. Die Daten werden nachher mit der gewählten Permutation permutiert und wieder in die Daten zurück geschrieben. Falls das jetztige Register das links oder rechtsäusserste Register ist, wird es stattdessen in einen temporären Speicher gespeichert. Dies führt man nun so oft durch, wie man Anzahl 256 Bit Register in der Partition hat. Falls es noch einen Rest hat, wird dieser auch in ein temporären Speicher geschrieben. Am Schluss wird noch der temporäre Speicher in die noch nicht überschriebenen Zellen gespeichert. Es gibt immer mindestens 2 256Bit Blöcke und 1 Rest Element, da ansonsten der Insertionsort übernimmt

### Allfällige Probleme / Einschränkungen ihres Codes
Leider ist mein Code noch nicht von allen Krankheiten befreit. Grundsätzlich sind nur vectors mit dem int Typen erlaubt. Ich habe dazu 3 Tests erstellt. Eins mit 8 Elementen, eins mit 68 und ein sehr grosses. Die zwei ersten besteht er(Der kleine Test triggert nur den Insertionsort). Der mittlere wird auch erfolgreich Sortiert. Beim grossen wirft er meistens einen Out of Bound Fehler oder sortiert nicht richtig. Neben den Out of Bound Fehler denke ich, das das Ereignis wenn alle kleiner sind als das Pivot Element, sowie wenn alle Elemente in der Partition gleich sind, wie das Pivot Element noch nicht gefixt sind. Leider fehlt mir hizu auch die Zeit. 
