1. Wypisz nagłówek (pierwszą linię) z iris.csv
    > `cat iris.csv | head -n 1`

2. Wypisz wszystkie linie oprócz pierwszej z iris.csv (podpowiedź: skorzystaj z man tail).
    > `cat iris.csv | tail -n +2`

3. Zamień gatunki na liczby (wyświetl poprawione linie, nie zmieniaj treści pliku):
    - “Setosa” na 1,
    - “Versicolor” na 2,
    - “Virginica” na 3.
    > `cat iris.csv | sed 's/Setosa/1/g' | sed 's/Versicolor/2/g' | sed 's/Virginica/3/g'`

4. Wypisz dostępne rodzaje irysów bez powtórzeń i bez cudzysłowów.
    > `cat iris.csv | cut -d ',' -f 5 | uniq | tr -d '"' | tail -n +2`

5. Oblicz sumę wartości dla każdego wiersza.
    > `cat iris.csv | LC_NUMERIC="C" awk -F, 'NR>1 {print $1+$2+$3+$4}'`

6. Oblicz średnią wartość drugiej kolumny (sepal.width).
    > `cat iris.csv | awk -F, 'NR>1 { sum += $2; n++ } END { if (n > 0) print sum / n; }'`

7. Wypisz całą linię, która ma maksymalną wartość czwartej kolumny (petal.width).
    > `cat iris.csv | awk -F, '{if($4 > max) {max=$4; maxline=$0}} END {print maxline}'`

8. Wypisz nazwę gatunku Irysa, którego pierwsza kolumna (sepal.length) jest większa niż 7.
    > `cat iris.csv | awk -F, '$1>7' | cut -d ',' -f 5`

9. Przeformatuj plik CSV korzystając z printf w awk na pola oddzielone tabulatorami obcinając liczby do całkowitych.
    > `cat iris.csv | awk -F, 'NR==1 {print; next} {printf "%d\t%d\t%d\t%d\t%s\n", $1, $2, $3, $4, $5}'`

10. Zmień losowo kolejność wierszy z danymi o irysach i zapisz je w nowym pliku CSV z nagłówkiem.
    > `shuf iris.csv > iris2.csv`