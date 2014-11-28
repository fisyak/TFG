<html>
<head>
<title>Total Cross Sections</title>
<link rel="stylesheet" type="text/css" href="pythia.css"/>
<link rel="shortcut icon" href="pythia32.gif"/>
</head>
<body>

<script language=javascript type=text/javascript>
function stopRKey(evt) {
var evt = (evt) ? evt : ((event) ? event : null);
var node = (evt.target) ? evt.target :((evt.srcElement) ? evt.srcElement : null);
if ((evt.keyCode == 13) && (node.type=="text"))
{return false;}
}

document.onkeypress = stopRKey;
</script>
<?php
if($_POST['saved'] == 1) {
if($_POST['filepath'] != "files/") {
echo "<font color='red'>SETTINGS SAVED TO FILE</font><br/><br/>"; }
else {
echo "<font color='red'>NO FILE SELECTED YET.. PLEASE DO SO </font><a href='SaveSettings.php'>HERE</a><br/><br/>"; }
}
?>

<form method='post' action='TotalCrossSections.php'>

<h2>Total Cross Sections</h2>

The <code>SigmaTotal</code> class returns the total, elastic, diffractive 
and nondiffractive cross sections in hadronic collisions, and also the
slopes of the <i>d(sigma)/dt</i> distributions. The parametrizations 
used are from [<a href="Bibliography.php" target="page">Sch97</a>] which borrows some of the total cross 
sections from [<a href="Bibliography.php" target="page">Don92</a>].

<p/>
The allowed combinations of incoming particles are <i>p + p</i>, 
<i>pbar + p</i>, <i>pi+ + p</i>, <i>pi- + p</i>, 
<i>pi0/rho0 + p</i>, <i>phi + p</i>, <i>J/psi + p</i>, 
<i>rho + rho</i>, <i>rho + phi</i>, <i>rho + J/psi</i>, 
<i>phi + phi</i>, <i>phi + J/psi</i>, <i>J/psi + J/psi</i>.   
The strong emphasis on vector mesons is related to the description
of <i>gamma + p</i> and <i>gamma + gamma</i> interactions in a 
Vector Dominance Model framework (which will not be available for some 
time to come, so this is a bit of overkill).

<h3>Variables</h3>

If the internally implemented cross section parametrizations are not 
satisfactory, it is possible to override the cross section values 
with  

<br/><br/><strong>SigmaTotal:setOwn</strong>  <input type="radio" name="1" value="on"><strong>On</strong>
<input type="radio" name="1" value="off"><strong>Off</strong>
 &nbsp;&nbsp;(<code>default = <strong>no</strong></code>)<br/>
Allow a user to set own cross sections by hand; yes/no = true/false.
  

<p/>
When <code>SigmaTotal:setOwn = yes</code>, the user is expected to set 
values for the corresponding cross sections:

<br/><br/><table><tr><td><strong>SigmaTotal:sigmaTot </td><td></td><td> <input type="text" name="2" value="80." size="20"/>  &nbsp;&nbsp;(<code>default = <strong>80.</strong></code>; <code>minimum = 0.</code>)</td></tr></table>
Total cross section in mb.
  

<br/><br/><table><tr><td><strong>SigmaTotal:sigmaEl </td><td></td><td> <input type="text" name="3" value="20." size="20"/>  &nbsp;&nbsp;(<code>default = <strong>20.</strong></code>; <code>minimum = 0.</code>)</td></tr></table>
Elastic cross section in mb.
  

<br/><br/><table><tr><td><strong>SigmaTotal:sigmaXB </td><td></td><td> <input type="text" name="4" value="8." size="20"/>  &nbsp;&nbsp;(<code>default = <strong>8.</strong></code>; <code>minimum = 0.</code>)</td></tr></table>
Single Diffractive cross section <i>A + B -> X + B</i> in mb.
  

<br/><br/><table><tr><td><strong>SigmaTotal:sigmaAX </td><td></td><td> <input type="text" name="5" value="8." size="20"/>  &nbsp;&nbsp;(<code>default = <strong>8.</strong></code>; <code>minimum = 0.</code>)</td></tr></table>
Single Diffractive cross section <i>A + B -> A + X</i> in mb.
  

<br/><br/><table><tr><td><strong>SigmaTotal:sigmaXX </td><td></td><td> <input type="text" name="6" value="4." size="20"/>  &nbsp;&nbsp;(<code>default = <strong>4.</strong></code>; <code>minimum = 0.</code>)</td></tr></table>
Double Diffractive cross section <i>A + B -> X_1 + X_2</i> in mb.
  

<p/>
Note that the total cross section subtracted by the elastic and various 
diffractive ones gives the inelastic nondiffractive cross section, 
which therefore is not set separately. If this cross section evaluates 
to be negative the internal parametrizations are used instead of the 
ones here. However, since the nondiffractive inelastic cross section 
is what makes up the minimum-bias event class, and plays a major role 
in the description of multiple interactions, it is important that a 
consistent set is used. 

<p/>
In the above option the <i>t</i> slopes are based on the internal
parametrizations. In addition there is no Coulomb-term contribution
to the elastic (or total) cross section, which of course becomes 
infinite if this contribution is included. If you have switched on
<code>SigmaTotal:setOwn</code> you can further switch on a machinery 
to include the Coulomb term, including interference with the conventional
strong-interaction Pomeron one [<a href="Bibliography.php" target="page">Ber87</a>]. Then the elastic cross 
section is no longer taken from <code>SigmaTotal:sigmaEl</code> but 
derived from the parameters below and <code>SigmaTotal:sigmaTot</code>, 
using the optical theorem. The machinery is only intended to be used for
<i>p p</i> and <i>pbar p</i> collisions. The description of 
diffractive events, and especially their slopes, remains unchanged. 

<br/><br/><strong>SigmaElastic:setOwn</strong>  <input type="radio" name="7" value="on"><strong>On</strong>
<input type="radio" name="7" value="off"><strong>Off</strong>
 &nbsp;&nbsp;(<code>default = <strong>no</strong></code>)<br/>
Allow a user to set parameters for the normalization and shape of the
elastic cross section the by hand; yes/no = true/false.
  

<br/><br/><table><tr><td><strong>SigmaElastic:bSlope </td><td></td><td> <input type="text" name="8" value="18." size="20"/>  &nbsp;&nbsp;(<code>default = <strong>18.</strong></code>; <code>minimum = 0.</code>)</td></tr></table>
the slope <i>b</i> of the strong-interaction term <i>exp(bt)</i>,
in units of GeV^-2. 
  

<br/><br/><table><tr><td><strong>SigmaElastic:rho </td><td></td><td> <input type="text" name="9" value="0.13" size="20"/>  &nbsp;&nbsp;(<code>default = <strong>0.13</strong></code>; <code>minimum = -1.</code>; <code>maximum = 1.</code>)</td></tr></table>
the ratio of the real to the imaginary parts of the nuclear scattering
amplitude.
  

<br/><br/><table><tr><td><strong>SigmaElastic:lambda </td><td></td><td> <input type="text" name="10" value="0.71" size="20"/>  &nbsp;&nbsp;(<code>default = <strong>0.71</strong></code>; <code>minimum = 0.1</code>; <code>maximum = 2.</code>)</td></tr></table>
the main parameter of the electric form factor
<i>G(t) = lambda^2 / (lambda + |t|)^2</i>, in units of GeV^2.
  

<br/><br/><table><tr><td><strong>SigmaElastic:tAbsMin </td><td></td><td> <input type="text" name="11" value="5e-5" size="20"/>  &nbsp;&nbsp;(<code>default = <strong>5e-5</strong></code>; <code>minimum = 1e-10</code>)</td></tr></table>
since the Coulomb contribution is infinite a lower limit on 
<i>|t|</i> must be set to regularize the divergence, 
in units of GeV^2.
  

<br/><br/><table><tr><td><strong>SigmaElastic:phaseConst </td><td></td><td> <input type="text" name="12" value="0.577" size="20"/>  &nbsp;&nbsp;(<code>default = <strong>0.577</strong></code>)</td></tr></table>
The Coulomb term is taken to contain a phase factor 
<i>exp(+- i alpha phi(t))</i>, with + for <i>p p</i> and - for 
<i>pbar p</i>, where <i>phi(t) = - phaseConst - ln(-B t/2)</i>.
This constant is model dependent [<a href="Bibliography.php" target="page">Cah82</a>].
  

<input type="hidden" name="saved" value="1"/>

<?php
echo "<input type='hidden' name='filepath' value='".$_GET["filepath"]."'/>"?>

<table width="100%"><tr><td align="right"><input type="submit" value="Save Settings" /></td></tr></table>
</form>

<?php

if($_POST["saved"] == 1)
{
$filepath = $_POST["filepath"];
$handle = fopen($filepath, 'a');

if($_POST["1"] != "no")
{
$data = "SigmaTotal:setOwn = ".$_POST["1"]."\n";
fwrite($handle,$data);
}
if($_POST["2"] != "80.")
{
$data = "SigmaTotal:sigmaTot = ".$_POST["2"]."\n";
fwrite($handle,$data);
}
if($_POST["3"] != "20.")
{
$data = "SigmaTotal:sigmaEl = ".$_POST["3"]."\n";
fwrite($handle,$data);
}
if($_POST["4"] != "8.")
{
$data = "SigmaTotal:sigmaXB = ".$_POST["4"]."\n";
fwrite($handle,$data);
}
if($_POST["5"] != "8.")
{
$data = "SigmaTotal:sigmaAX = ".$_POST["5"]."\n";
fwrite($handle,$data);
}
if($_POST["6"] != "4.")
{
$data = "SigmaTotal:sigmaXX = ".$_POST["6"]."\n";
fwrite($handle,$data);
}
if($_POST["7"] != "no")
{
$data = "SigmaElastic:setOwn = ".$_POST["7"]."\n";
fwrite($handle,$data);
}
if($_POST["8"] != "18.")
{
$data = "SigmaElastic:bSlope = ".$_POST["8"]."\n";
fwrite($handle,$data);
}
if($_POST["9"] != "0.13")
{
$data = "SigmaElastic:rho = ".$_POST["9"]."\n";
fwrite($handle,$data);
}
if($_POST["10"] != "0.71")
{
$data = "SigmaElastic:lambda = ".$_POST["10"]."\n";
fwrite($handle,$data);
}
if($_POST["11"] != "5e-5")
{
$data = "SigmaElastic:tAbsMin = ".$_POST["11"]."\n";
fwrite($handle,$data);
}
if($_POST["12"] != "0.577")
{
$data = "SigmaElastic:phaseConst = ".$_POST["12"]."\n";
fwrite($handle,$data);
}
fclose($handle);
}

?>
</body>
</html>

<!-- Copyright (C) 2008 Torbjorn Sjostrand -->
