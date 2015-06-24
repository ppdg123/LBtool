#Labeling tools#

##configuration##

Please modify LBtool.conf before labeling

* For rects

```

rec
POINT1(name for the point)
POINT2
...

```
2*n points.

* For points

```

point
POINT1
POINT2
...

```

* For superpixels

```

point
CATEGORY1
CATEGORY2
...

```

##Operate##

* 'n' : Next image (without saving current)
* 'p' : Previous image (without saving current)
* space : Saving current result and go to next one
* enter : Saveing current result and back to previous one
* '-' : modify previous point
* '=' : modify next point.

_Note _: In superpixel labeling tasks, use  '-' and '=' for changing category.