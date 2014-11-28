<html>
<head>
<title>HepMC Interface</title>
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

<form method='post' action='HepMCInterface.php'>

<h2>HepMC Interface</h2>

An interface to the HepMC [<a href="Bibliography.php" target="page">Dob01</a>] standard event record 
format has been provided by M. Kirsanov. To use it, the relevant 
libraires need to be linked, as explained in the <code>README</code> 
file. Only version 2 of HepMC is supported. (Version 1 requires 
a different interface structure, which was only supported up until 
Pythia 8.107.) 

<p/>
The (simple) procedure to translate PYTHIA 8 events into HepMC ones 
is illustrated in the <code>main31.cc</code>, <code>main32.cc</code>   
and <code>main54.cc</code> main programs. At the core is a call to
the
<pre>
HepMC::I_Pythia8::fill_next_event( pythia, hepmcevt, ievnum = -1, convertGluonTo0 = false ) 
</pre>
which takes a reference of the generator object and uses it, on the one
hand, to read out and covert the event record in <code>pythia.event</code> 
and, on the other hand, to extract and store parton-density (PDF) 
information for the hard subprocess from <code>pythia.info</code>. 
The optional last argument, if <code>true</code>, allows you to store 
gluons as "PDG" code 0 rather than the normal 21; this only applies to 
the PDF information, not the event record. 

<p/>
The earlier version of this routine,
<pre>
HepMC::I_Pythia8::fill_next_event( pythia.event, hepmcevt, ievnum = -1 ) 
</pre>
is retained (for now) for backwards compatibility. It takes a PYTHIA event 
as input and returns a HepMC one, but without storing the PDF information. 
The latter could then instead be stored by a separate call
<pre>
HepMC::I_Pythia8::pdf_put_info( hepmcevt, pythia, convertGluonTo0 = false ) 
</pre>
or not, as wished.


</body>
</html>

<!-- Copyright (C) 2008 Torbjorn Sjostrand -->
