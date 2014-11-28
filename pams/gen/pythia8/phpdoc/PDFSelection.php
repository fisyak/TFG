<html>
<head>
<title>PDF Selection</title>
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

<form method='post' action='PDFSelection.php'>

<h2>PDF Selection</h2>

This page contains three subsections. The first deals with how to 
pick  the parton distribution set for protons, including from LHAPDF, 
to be used for all proton and antiproton beams. The second is a special
option that allows a separate PDF set to be used for the hard process
only, while the first choice would still apply to everything else.
The third gives the possibility to switch off the lepton 
"parton density". 

<h3>Parton densities for protons</h3>

The selection of parton densities is made once and then is propagated 
through the program. It is essential to make an informed choice, 
for several reasons: 
<br/><b>Warning 1:</b> the choice of PDF set affects a number of
properties of events. A change of PDF therefore requires a complete 
retuning e.g.  of the multiple-interactions model for minimum-bias and 
underlying events.
<br/><b>Warning 2:</b> People often underestimate the differences 
between different sets on the market. The sets for the same order are 
constructed to behave more or less similarly at large <i>x</i> and 
<i>Q^2</i>, while the multiple interactions are dominated by the 
behaviour in the region of small <i>x</i> and <i>Q^2</i>. A good 
PDF parametrization ought to be sensible down to <i>x = 10^-6</i> 
(<i>x = 10^-7</i>) and <i>Q^2 = 1</i> GeV^2 for Tevatron (LHC) 
applications. Unfortunately there are distributions on the market that 
completely derail in that region. The <code>main41.cc</code> and 
<code>main42.cc</code> programs in the <code>examples</code> 
subdirectory provide some examples of absolutely minimal sanity checks 
before a new PDF set is put in production.
<br/><b>Warning 3:</b> NLO and LO sets tend to have quite different
behaviours, e.g. NLO ones have less gluons at small x, which then is 
compensated by positive corrections in the NLO matrix elements.
Therefore do not blindly assume that an NLO tune has to be better than 
an LO one when combined with the LO matrix elements in PYTHIA. There are 
explicit examples where such thinking can lead you down the wrong alley.

<p/>
The simplest option is to pick one 
of the few distributions available internally:

<br/><br/><table><tr><td><strong>PDF:pSet  </td><td>  &nbsp;&nbsp;(<code>default = <strong>2</strong></code>; <code>minimum = 1</code>; <code>maximum = 2</code>)</td></tr></table>
Parton densities to be used for proton beams (and, by implication,
antiproton ones):
<br/>
<input type="radio" name="1" value="1"><strong>1 </strong>: GRV 94 L;<br/>
<input type="radio" name="1" value="2" checked="checked"><strong>2 </strong>: CTEQ 5 L.<br/>

<p/>
Obviously this choice is mainly intended to get going, and if you link to
the <a href="http://projects.hepforge.org/lhapdf/" target="page">LHAPDF 
library</a> [<a href="Bibliography.php" target="page">Wha05</a>] you get access to a much wider selection.
<br/><b>Warning:</b> owing to previous problems with the behaviour of PDF's
beyond the <i>x</i> and <i>Q^2</i> boundaries of a set, you should
only use LHAPDF <b>version 5.3.0 or later</b>.

<br/><br/><strong>PDF:useLHAPDF</strong>  <input type="radio" name="2" value="on"><strong>On</strong>
<input type="radio" name="2" value="off" checked="checked"><strong>Off</strong>
 &nbsp;&nbsp;(<code>default = <strong>off</strong></code>)<br/>
If off then the choice of proton PDF is based on <code>pPDFset</code>
above. If on then it is instead based on the choice of 
<code>LHAPDFset</code> and <code>LHAPDFmember</code> below.
<br/><b>Note:</b> in order for this option to work you must have 
compiled PYTHIA appropriately and have set the <code>LHAPATH</code> 
environment variable to provide the data-files directory of your local 
LHAPDF installation. See the README file in the <code>examples</code> 
directory for further instructions. 
  

<br/><br/><table><tr><td><strong>PDF:LHAPDFset  </td><td></td><td> <input type="text" name="3" value="MRST2004FF4lo.LHgrid" size="20"/>  &nbsp;&nbsp;(<code>default = <strong>MRST2004FF4lo.LHgrid</strong></code>)</td></tr></table>
Name of proton PDF set from LHAPDF to be used. You have to choose 
from the 
<a href="http://projects.hepforge.org/lhapdf/pdfsets" target="page">
list of available sets</a>. Examples of some recent ones would be 
cteq61.LHpdf, cteq61.LHgrid, cteq6l.LHpdf, cteq6ll.LHpdf, 
MRST2004nlo.LHpdf, MRST2004nlo.LHgrid, MRST2004nnlo.LHgrid and 
MRST2004FF3lo.LHgrid. If you pick a LHpdf set it will require some 
calculation the first time it is called. 
<br/><b>Technical note:</b> if you provide a name beginning with a 
slash (/) it is assumed you want to provide the full file path and then
<code>initPDFsetM(name)</code> is called, else the correct path is assumed 
already set and <code>initPDFsetByNameM(name)</code> is called.
   

<br/><br/><table><tr><td><strong>PDF:LHAPDFmember  </td><td></td><td> <input type="text" name="4" value="0" size="20"/>  &nbsp;&nbsp;(<code>default = <strong>0</strong></code>; <code>minimum = 0</code>)</td></tr></table>
Further choice of a specific member from the set picked above. Member 0
should normally correspond to the central value, with higher values
corresponding to different error PDF's somewhat off in different 
directions. You have to check from set to set which options are open.
<br/><b>Note:</b> you can only use one member in a run, so if you
want to sweep over many members you either have to do many separate
runs or, as a simplification, save the 
<?php $filepath = $_GET["filepath"];
echo "<a href='EventInformation.php?filepath=".$filepath."' target='page'>";?>pdf weights</a> at the hard scattering
and do an offline reweighting of events.
     

<br/><br/><strong>PDF:extrapolateLHAPDF</strong>  <input type="radio" name="5" value="on"><strong>On</strong>
<input type="radio" name="5" value="off" checked="checked"><strong>Off</strong>
 &nbsp;&nbsp;(<code>default = <strong>off</strong></code>)<br/>
Parton densities have a guaranteed range of validity in <i>x</i>
and <i>Q^2</i>, and what should be done beyond that range usually is 
not explained by the authors of PDF sets. Nevertheless these boundaries
very often are exceeded, e.g. minimum-bias studies at LHC may sample
<i>x</i> values down to <i>10^-8</i>, while many PDF sets stop
already at <i>10^-5</i>. The default behaviour is then that the 
PDF's are frozen at the boundary, i.e. <i>xf(x,Q^2)</i> is fixed at
its value at <i>x_min</i> for all values <i>x &lt; x_min</i>,
and so on. This is a conservative approach. Alternatively, if you
switch on extrapolation, then parametrizations will be extended beyond
the boundaries, by some prescription. In some cases this will provide a
more realistic answer, in others complete rubbish. Another problem is 
that some of the PDF-set codes will write a warning message anytime the
limits are exceeded, thus swamping your output file. Therefore you should 
study a set seriously before you run it with this switch on.
  

<p/> 
If you want to use PDF's not found in LHAPDF, or you want to interface
LHAPDF another way, you have full freedom to use the more generic 
<?php $filepath = $_GET["filepath"];
echo "<a href='PartonDistributions.php?filepath=".$filepath."' target='page'>";?>interface options</a>.

<h3>Parton densities for protons in the hard process</h3>

The above options provides a PDF set that will be used everywhere:
for the hard process, the parton showers and the multiple interactions
alike. As already mentioned, therefore a change of PDF should be
accompanied by a <b>complete</b> retuning of the whole MI framework,
and maybe more. There are cases where one may want to explore 
different PDF options for the hard process, but would not want to touch 
the rest. If several different sets are to be compared, a simple
reweighting based on the <?php $filepath = $_GET["filepath"];
echo "<a href='EventInformation.php?filepath=".$filepath."' target='page'>";?>originally 
used</a> flavour, <i>x</i>, <i>Q^2</i> and PDF values may offer the 
best route. The options in this section allow a choice of the PDF set
for the hard process alone, while the choice made in the previous section
would still be used for everything else. The hardest interaction
of the minimum-bias process is part of the multiple-interactions
framework and so does not count as a hard process here. 

<p/>
Of course it is inconsistent to use different PDF's in different parts 
of an event, but if the <i>x</i> and <i>Q^2</i> ranges mainly accessed 
by the components are rather different then the contradiction would not be
too glaring. Furthermore, since standard PDF's are one-particle-inclusive
we anyway have to 'invent' our own PDF modifications to handle configurations
where more than one parton is kicked out of the proton [<a href="Bibliography.php" target="page">Sjo04</a>]. 

<p/>
The PDF choices that can be made are the same as above, so we do not 
repeat the detailed discussion.

<br/><br/><strong>PDF:useHard</strong>  <input type="radio" name="6" value="on"><strong>On</strong>
<input type="radio" name="6" value="off" checked="checked"><strong>Off</strong>
 &nbsp;&nbsp;(<code>default = <strong>off</strong></code>)<br/>
If on then select a separate PDF set for the hard process, using the 
variables below. If off then use the same PDF set for everything,
as already chosen above.   
  

<br/><br/><table><tr><td><strong>PDF:pHardSet  </td><td>  &nbsp;&nbsp;(<code>default = <strong>2</strong></code>; <code>minimum = 1</code>; <code>maximum = 2</code>)</td></tr></table>
Parton densities to be used for proton beams (and, by implication,
antiproton ones):
<br/>
<input type="radio" name="7" value="1"><strong>1 </strong>: GRV 94 L;<br/>
<input type="radio" name="7" value="2" checked="checked"><strong>2 </strong>: CTEQ 5 L.<br/>

<br/><br/><strong>PDF:useHardLHAPDF</strong>  <input type="radio" name="8" value="on"><strong>On</strong>
<input type="radio" name="8" value="off" checked="checked"><strong>Off</strong>
 &nbsp;&nbsp;(<code>default = <strong>off</strong></code>)<br/>
If off then the choice of proton PDF is based on <code>hardpPDFset</code>
above. If on then it is instead based on the choice of 
<code>hardLHAPDFset</code> and <code>hardLHAPDFmember</code> below.
  

<br/><br/><table><tr><td><strong>PDF:hardLHAPDFset  </td><td></td><td> <input type="text" name="9" value="MRST2004FF4lo.LHgrid" size="20"/>  &nbsp;&nbsp;(<code>default = <strong>MRST2004FF4lo.LHgrid</strong></code>)</td></tr></table>
Name of proton PDF set from LHAPDF to be used. 
   

<br/><br/><table><tr><td><strong>PDF:hardLHAPDFmember  </td><td></td><td> <input type="text" name="10" value="0" size="20"/>  &nbsp;&nbsp;(<code>default = <strong>0</strong></code>; <code>minimum = 0</code>)</td></tr></table>
Further choice of a specific member from the set picked above. 
     

<p/>
Note that there is no separate equivalent of the 
<code>PDF:extrapolateLHAPDF</code> flag specifically for the hard
PDF. Since LHAPDF only has one global flag for extrapolation or not,
the choice for the normal PDF's also applies to the hard ones.

<h3>Parton densities for leptons</h3>

For electrons/leptons there is no need to choose between different 
parametrizations, since only one implementation is available, and 
should be rather uncontroversial (apart from some technical details).
However, insofar as e.g. <i>e^+ e^-</i> data often are corrected 
back to a world without any initial-state photon radiation, it is 
useful to have a corresponding option available here.

<br/><br/><strong>PDF:lepton</strong>  <input type="radio" name="11" value="on" checked="checked"><strong>On</strong>
<input type="radio" name="11" value="off"><strong>Off</strong>
 &nbsp;&nbsp;(<code>default = <strong>on</strong></code>)<br/>
Use parton densities for lepton beams or not. If off the colliding
leptons carry the full beam energy, if on part of the energy is 
radiated away by initial-state photons. In the latter case the
initial-state showers will generate the angles and energies of the
set of photons that go with the collision. In addition one collinear
photon per beam carries any leftover amount of energy not described
by shower emissions. If the initial-state showers are switched off 
these collinear photons will carry the full radiated energy.  
   

<h3>Incoming parton selection</h3>

There is one useful degree of freedom to restrict the set of incoming 
quark flavours for hard processes. It does not change the PDF's as such, 
only which quarks are allowed to contribute to the hard-process cross 
sections. Note that separate but similarly named modes are available 
for multiple interactions and spacelike showers.

<br/><br/><table><tr><td><strong>PDFinProcess:nQuarkIn  </td><td></td><td> <input type="text" name="12" value="5" size="20"/>  &nbsp;&nbsp;(<code>default = <strong>5</strong></code>; <code>minimum = 0</code>; <code>maximum = 5</code>)</td></tr></table>
Number of allowed incoming quark flavours in the beams; a change 
to 4 would thus exclude <i>b</i> and <i>bbar</i> as incoming 
partons, etc.
  

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

if($_POST["1"] != "2")
{
$data = "PDF:pSet = ".$_POST["1"]."\n";
fwrite($handle,$data);
}
if($_POST["2"] != "off")
{
$data = "PDF:useLHAPDF = ".$_POST["2"]."\n";
fwrite($handle,$data);
}
if($_POST["3"] != "MRST2004FF4lo.LHgrid")
{
$data = "PDF:LHAPDFset = ".$_POST["3"]."\n";
fwrite($handle,$data);
}
if($_POST["4"] != "0")
{
$data = "PDF:LHAPDFmember = ".$_POST["4"]."\n";
fwrite($handle,$data);
}
if($_POST["5"] != "off")
{
$data = "PDF:extrapolateLHAPDF = ".$_POST["5"]."\n";
fwrite($handle,$data);
}
if($_POST["6"] != "off")
{
$data = "PDF:useHard = ".$_POST["6"]."\n";
fwrite($handle,$data);
}
if($_POST["7"] != "2")
{
$data = "PDF:pHardSet = ".$_POST["7"]."\n";
fwrite($handle,$data);
}
if($_POST["8"] != "off")
{
$data = "PDF:useHardLHAPDF = ".$_POST["8"]."\n";
fwrite($handle,$data);
}
if($_POST["9"] != "MRST2004FF4lo.LHgrid")
{
$data = "PDF:hardLHAPDFset = ".$_POST["9"]."\n";
fwrite($handle,$data);
}
if($_POST["10"] != "0")
{
$data = "PDF:hardLHAPDFmember = ".$_POST["10"]."\n";
fwrite($handle,$data);
}
if($_POST["11"] != "on")
{
$data = "PDF:lepton = ".$_POST["11"]."\n";
fwrite($handle,$data);
}
if($_POST["12"] != "5")
{
$data = "PDFinProcess:nQuarkIn = ".$_POST["12"]."\n";
fwrite($handle,$data);
}
fclose($handle);
}

?>
</body>
</html>

<!-- Copyright (C) 2008 Torbjorn Sjostrand -->
