1. Usunięty plik, ale z uruchomionym terminalem, jest nadal zapisany w pamięci procesora

2. Komendą:
    > pgrep -f <nazwa_pliku_python>
można wyciągnąć numer PID jego procesu

3. Znając PID, można komendą:
    > ls -l /proc/<PID>/fd
znaleźć numer wykonawczy stworzonego a potem usuniętego pliku

4. Plik można skopiować z bufora, komendą:
    > cp /proc/<PID>/fd/<numer_FD> <nowy_plik.txt>