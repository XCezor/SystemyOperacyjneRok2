1. Podaj polecenia, które:
    - nadpisują określony tekst do pliku bez użycia edytora.
        > echo "tekst" > nazwa_pliku
    - dodają określony tekst na koniec pliku bez użycia edytora.
        > echo "Nowy tekst" >> nazwa_pliku

2. Wymień skrót, który komentuje lub usuwa komentarz linii w nano.
    > Alt+#

3. Wymień skróty, który dodają i odejmują wcięcie do bieżącej linii w nano.
    > Dodanie wcięcia (przesunięcie w prawo): Alt+]
    > Usunięcie wcięcia (przesunięcie w lewo): Alt+[
    > Dodatkowo: pojedyncze wcięcie można wstawić klawiszem Tab; usunięcie może być przez Backspace lub Shift+Tab gdy terminal to obsługuje.

4. Podaj nazwę pliku konfiguracyjnego nano i treść, która umożliwi, aby tabulatory były automatycznie zamieniane na spacje (za każdym uruchomieniem).
    > Plik: ~/.nanorc
    > Przykładowa zawartość (zamienia tabulatory na spacje, ustawia szerokość tabulacji na 4 spacje):
       - set tabstospaces
       - set tabsize 4

5. Podaj nazwę pliku konfiguracyjnego vim i treść, która umożliwi, aby tabulatory były automatycznie zamieniane na spacje (za każdym uruchomieniem).
    > Plik: ~/.vimrc
    > Przykładowa zawartość (ustawienia dla zamiany tabów na 4 spacje):
        - set expandtab
        - set tabstop=4
        - set shiftwidth=4
        - set softtabstop=4