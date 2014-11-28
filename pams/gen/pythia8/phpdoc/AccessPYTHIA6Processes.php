<html>
<head>
<title>Accessing Pythia6 Processes</title>
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

<form method='post' action='AccessPYTHIA6Processes.php'>

<h2>Access PYTHIA 6 Processes</h2>

Gradually all relevant processes from PYTHIA 6 are being 
re-implemented in PYTHIA 8, but this transition is not finished.
For a while longer it may therefore at times be convenient to 
access the Fortran PYTHIA  6 process library. In order to give 
this access at runtime, and not only by writing/reading event files,
an interface is provided to C++. This interface is residing in 
<code>Pythia6Interface.h</code>, and in addition the PYTHIA 6 library
must be linked. The latter should normally be the most recent 
Fortran PYTHIA version, but must be at least 6.314, since this is 
the first version that allows processes to be output in the Les 
Houches format (and not only input).

<p/>
The  routines interfaced are
<ul>
<li><code>pygive(command)</code> to give in a command to change a 
setting, e.g. to decide which processes should be generated,
<li><code>pyinit( frame, beam, target, wIn)</code> to initialize 
the event generation chain,</li>
<li><code>pyupin()</code> to fill this initialization information 
in the <code>HEPRUP</code> commonblock,</li>
<li><code>pyupev()</code> to generate the next hard process and 
fill the event information in the <code>HEPEUP</code> commonblock,</li>
<li><code>pylist( mode)</code> to list the event at the process 
level,</li>
<li><code>pystat( mode)</code> to print statistics on the event 
generation process.</li>
</ul> 
Details on allowed arguments are given in the PYTHIA 6.4 manual.

<p/>
These methods can be used in context of the  
<?php $filepath = $_GET["filepath"];
echo "<a href='LesHouchesAccord.php?filepath=".$filepath."' target='page'>";?><code>LHAupFortran</code></a> class. 
The existing code there takes care of converting <code>HEPRUP</code> 
and <code>HEPEUP</code> commonblock information from Fortran to C++,
and of making it available to the PYTHIA 8 methods. What needs to be 
supplied are the two <code>LHAupFortran::fillHepRup()</code> and
<code>LHAupFortran::fillHepEup()</code> methods. The first can
contain an arbitrary number of <code>pygive(...)</code>, followed
by <code>pyinit(...)</code> and <code>pyupin()</code> in that order.
The second only needs to contain <code>pyupev()</code>. Finally,
the use of <code>pylist(...)</code> and <code>pystat(...)</code>
is entirely optional, and calls are best put directly in the
main program.

<p/>
This means that all other Fortran routines have not been interfaced
and cannot be accessed directly from the C++ code; there is no need
for them in the current setup.

<p/>
PYTHIA 6.4 does its own rejection of events internally, according to
the strategy option 3. However, the current runtime interface does not 
take cross-section information from PYTHIA 6.4. This means that both
the initial maxima and the final cross sections printed by the PYTHIA 8
routines are irrelevant in this case. Instead you have to study the
standard PYTHIA 6.4 initialization printout and call on 
<code>pystat(...)</code> at the end of the run to obtain this information. 
It also means that you cannot mix with internally generated events,
unlike what could have been allowed for strategy 3. Should a strong need 
arise, PYTHIA 6.4 could be modified to work with strategy option 1 and
thus allow a mixing with internal processes, but we do not expect this
to happen.
 
<p/>
An example of a <code>fillHepRup()</code> method to set up 
<i>Z^0</i> production at LHC, with masses above 50 GeV, would be
<pre>
bool LHAinitFortran::fillHepRup() { 
  Pythia6Interface::pygive("msel = 11"); 
  Pythia6Interface::pygive("ckin(1) = 50."); 
  Pythia6Interface::pyinit("cms","p","p",14000.);   
  Pythia6Interface::pyupin();
  return true;
}
</pre>
and the process-generic <code>fillHepEup()</code> method would be
<pre>
bool LHAupFortran::fillHepEup() { 
  Pythia6Interface::pyupev();
  return true;
}
</pre>
Note that, of all parameters that could be set with the 
<code>PYGIVE</code>, only those that influence the generation of the 
hard processes have any impact, since this is the only part of the 
Fortran code that is used. Also, if you want to set many parameters,
you can easily collect them in one file (separate from PYTHIA 8
input) and parse that file.

<p/>
All hard PYTHIA 6 processes should be available for full generation
in PYTHIA 8, at least to the extent that they are defined for beams 
of protons and antiprotons, which is the key application for PYTHIA 8 
so far. Soft processes, i.e. elastic and diffractive scattering, as well 
as minimum-bias events, require a different kinematics machinery, and 
can only be generated with the internal PYTHIA 8 processes.

<p/>
A simple example is found in <code>main51.cc</code>, another with parsed 
input in <code>main52.cc</code> and a third with HepMC output in
<code>main54.cc</code>.

</body>
</html>

<!-- Copyright (C) 2008 Torbjorn Sjostrand -->
