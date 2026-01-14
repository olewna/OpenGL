# LAB11

## ZADANIE 01

### podłoże

Zaimplementuj w swoim projekcie mechanizm poruszania się dowolnego obiektu (nazywać będziemy go postacią) po nierównej siatce na przykładzie projektu ground z wykładu (wykorzystującego klasy `CGround` oraz `CPlayer`). W szczególności:

- niech klawisze `A` oraz `D` (lub za pomocą myszy) obracają obiektem w lewo i prawo, natomiast klawisze `W` oraz `S` przesuwają obiektem do przodu i do tyłu w kierunku wektora `Direction`,
- spraw, aby postać nie mogła poruszać się tam, gdzie nie ma żadnych obiektów pod nogami - w ten sposób możesz zaimplementować prosty system uniemożliwiający wchodzenie w pewne miejsca sceny (np. jezioro, woda),
- spraw, aby postać nie mogła podchodzić pod zbyt strome płaszczyzny (np. stroma góra).

## ZADANIE 02

### rozmieszczenie obiektów

Korzystając z projektu ground spraw, aby na scenie pojawiły się w losowych miejscach drzewa (lub inne obiekty, które będą pełniły rolę obiektów kolizyjnych) o różnej wielkości (zobacz przykład) oraz obiekty typu kwiaty/trawa (które nie będą uczestniczyły w problemie kolizji).

## ZADANIE 03

### piętra

Rozbuduj mechanizm poruszania się po podłożu złożonym z poziomów (wiele płaszczyzn jedna nad drugą - piętra)

#### modele

- ground-large.obj
- scene-large.obj
- scene-levels.obj
- scene-plane.obj
