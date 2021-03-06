dEdxPoint_t dEdxZ[] = {
//           p     dP          a                da           mu           dmu              sigma        dsigma
  {"e-  ", 0,0.185, 0.03,        65.5292 ,        1.8922 ,    -0.0349257,     0.00134765 ,     0.0566064,     0.00100984},
  {"e-  ", 0,0.25 , 0.1 ,       232.839  ,        5.16944,    -0.0372765,     0.00144041 ,     0.0607809,     0.000641246},
  {"e-  ", 0,0.35 , 0.1 ,       130.971  ,        2.56265,    -0.0328487,     0.000991585,     0.0618999,     0.000747657},
  {"e-  ", 0,0.45 , 0.1 ,        79.5933 ,        1.99336,    -0.035272 ,     0.00130774 ,     0.0641057,     0.00104665},
  {"e-  ", 0,0.55 , 0.1 ,        51.5762 ,        1.65354,    -0.0451886,     0.00163559 ,     0.0629263,     0.00137018},
  {"e-  ", 0,0.7  , 0.2 ,        53.501  ,        1.67808,    -0.0449293,     0.00172264 ,     0.0667238,     0.0015191},
  {"pbar", 1,0.35 , 0.1 ,       9.44101  ,      0.64836  ,   -0.0736221 ,    0.00494893  ,    0.0914034 ,    0.00533526},
  {"pbar", 1,0.45 , 0.1 ,       42.6756  ,      1.33024  ,    -0.100012 ,    0.00204792  ,    0.0810228 ,     0.0017096},
  {"pbar", 1,0.55 , 0.1 ,       61.9734  ,      1.69672  ,    -0.118131 ,     0.0016088  ,    0.0729614 ,    0.00133174},
  {"pbar", 1,0.65 , 0.1 ,       71.4414  ,      1.89982  ,    -0.109747 ,    0.00146086  ,    0.0698557 ,    0.00125934},
  {"pbar", 1,0.75 , 0.1 ,         78.09  ,      2.00024  ,   -0.0945546 ,    0.00131339  ,    0.0651208 ,    0.00108009},
  {"pbar", 1,0.85 , 0.1 ,       77.9105  ,      1.94997  ,   -0.0769022 ,    0.00127499  ,     0.062034 ,   0.000914223},
  {"pbar", 1,0.95 , 0.1 ,       67.6133  ,      1.83642  ,   -0.0620576 ,    0.00139861  ,    0.0644139 ,    0.00110119},
  {"pbar", 1,1.05 , 0.1 ,       61.1423  ,      1.71014  ,   -0.0440476 ,    0.00149434  ,    0.0654615 ,     0.0011357},
  {"pbar", 1,1.15 , 0.1 ,       57.4724  ,      1.73392  ,   -0.0328742 ,    0.00149553  ,    0.0618792 ,    0.00118101},
  {"pbar", 1,1.25 , 0.1 ,       50.2002  ,      1.69101  ,    -0.023037 ,    0.00158067  ,    0.0614529 ,    0.00138177},
  {"pbar", 1,1.35 , 0.1 ,       42.7658  ,      1.51336  ,   -0.0128077 ,    0.00175409  ,    0.0619163 ,    0.00144355},
  {"pbar", 1,1.45 , 0.1 ,       36.0411  ,      1.44285  ,  -0.00706468 ,    0.00194205  ,    0.0613535 ,     0.0017445},
  {"pbar", 1,1.55 , 0.1 ,       30.1695  ,      1.27214  ,  0.000579276 ,    0.00210916  ,    0.0614492 ,    0.00174457},
  {"pbar", 1, 1.8 , 0.4 ,       81.2691  ,      2.02509  ,    0.0160024 ,     0.0012701  ,    0.0601972 ,   0.000904521},
  {"pbar", 1,2.25 , 0.5 ,       44.5037  ,        1.605  ,    0.0275019 ,    0.00166932  ,    0.0588097 ,    0.00141286},
  {"pbar", 1,2.75 , 0.5 ,       14.8441  ,      0.96039  ,     0.040925 ,    0.00295314  ,    0.0575282 ,    0.00265508},
  {"K-  ", 2,0.35 , 0.1 ,       46.2246  ,      1.52475  ,    -0.094352 ,    0.00238168  ,    0.0720033 ,    0.00212505},
  {"K-  ", 2,0.45 , 0.1 ,       75.2919  ,      1.96599  ,   -0.0731439 ,    0.00151683  ,    0.0687379 ,    0.00137288},
  {"K-  ", 2,0.55 , 0.1 ,       92.9371  ,      2.24818  ,   -0.0509604 ,    0.00122663  ,    0.0640872 ,    0.00107438},
  {"K-  ", 2,0.65 , 0.1 ,       101.634  ,      2.37187  ,   -0.0309796 ,    0.00111671  ,    0.0615621 ,   0.000948677},
  {"K-  ", 2,0.75 , 0.1 ,       97.6744  ,      2.28391  ,   -0.0100392 ,    0.00113651  ,    0.0619761 ,   0.000928951},
  {"K-  ", 2,0.85 , 0.1 ,       87.0232  ,      2.12935  ,  0.000461286 ,    0.00121379  ,    0.0612678 ,   0.000948411},
  {"K-  ", 2,0.95 , 0.1 ,       77.1123  ,      1.99109  ,    0.0105619 ,    0.00126541  ,    0.0599465 ,   0.000934668},
  {"K-  ", 2,1.05 , 0.1 ,       61.3517  ,      1.83441  ,    0.0232235 ,    0.00146685  ,    0.0585052 ,    0.00110753},
  {"K-  ", 2,1.15 , 0.1 ,       53.8218  ,      1.70484  ,    0.0278963 ,    0.00150544  ,    0.0569979 ,     0.0011042},
  {"K-  ", 2,1.25 , 0.1 ,       44.1642  ,      1.52884  ,    0.0327157 ,    0.00170726  ,    0.0599256 ,    0.00132544},
  {"K-  ", 2,1.35 , 0.1 ,       37.7064  ,      1.41237  ,    0.0373272 ,    0.00181494  ,    0.0584703 ,    0.00136662},
  {"K-  ", 2,1.45 , 0.1 ,       31.1899  ,      1.33413  ,     0.043004 ,    0.00195101  ,    0.0565383 ,    0.00155681},
  {"K-  ", 2,1.55 , 0.1 ,       23.1877  ,      1.11795  ,    0.0453021 ,    0.00242521  ,    0.0616587 ,    0.00203264},
  {"pi- ", 3,0.35 , 0.1 ,       2109.85  ,       10.086  ,    0.0359854 ,   0.000244185  ,    0.0610198 ,   0.000168799},
  {"pi- ", 3,0.45 , 0.1 ,       1803.76  ,      9.38319  ,    0.0406214 ,   0.000267678  ,    0.0614388 ,   0.000187601},
  {"pi- ", 3,0.55 , 0.1 ,       1442.89  ,      8.60338  ,     0.038668 ,   0.000293938  ,    0.0595871 ,   0.000211454},
  {"pi- ", 3,0.65 , 0.1 ,       1144.06  ,      7.67973  ,     0.033845 ,   0.000320318  ,    0.0571641 ,   0.000220001},
  {"pi- ", 3,0.75 , 0.1 ,       867.115  ,      6.70874  ,    0.0315085 ,   0.000366638  ,     0.056816 ,   0.000252542},
  {"pi- ", 3,0.85 , 0.1 ,       662.762  ,       5.9064  ,    0.0312266 ,   0.000416138  ,    0.0565408 ,   0.000293211},
  {"pi- ", 3,0.95 , 0.1 ,       486.418  ,      5.03024  ,    0.0294613 ,   0.000494883  ,    0.0567906 ,   0.000341588},
  {"pi- ", 3,1.05 , 0.1 ,       380.428  ,      4.54895  ,    0.0277121 ,   0.000564498  ,    0.0561594 ,    0.00040877},
  {"pi- ", 3,1.15 , 0.1 ,       292.869  ,      3.94487  ,    0.0236639 ,   0.000624113  ,    0.0550911 ,   0.000430649},
  {"pi- ", 3,1.25 , 0.1 ,       219.213  ,      3.34668  ,     0.020406 ,   0.000725977  ,    0.0554348 ,   0.000475318},
  {"pi- ", 3,1.35 , 0.1 ,       172.327  ,      3.07517  ,    0.0192716 ,   0.000834018  ,    0.0555704 ,   0.000615196},
  {"pi- ", 3,1.45 , 0.1 ,       130.836  ,      2.73183  ,    0.0192672 ,   0.000976026  ,    0.0560413 ,   0.000772763},
  {"pi- ", 3,1.55 , 0.1 ,       101.703  ,      2.37004  ,    0.0135858 ,    0.00106553  ,    0.0535069 ,   0.000752896}
};
