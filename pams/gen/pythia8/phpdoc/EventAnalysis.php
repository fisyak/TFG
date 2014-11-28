<html>
<head>
<title>Event Analysis</title>
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

<form method='post' action='EventAnalysis.php'>

<h2>Event Analysis</h2>

<h3>Introduction</h3>

The routines in this section are intended to be used to analyze
event properties. As such they are not part of the main event
generation chain, but can be used in comparisons between Monte
Carlo events and real data. They are rather free-standing, but 
assume that input is provided in the PYTHIA 8 
<code>Event</code> format, and use a few basic facilities such 
as four-vectors.

<h3>Sphericity</h3>

The standard sphericity tensor is
<br/><i>
    S^{ab} = (sum_i p_i^a p_i^b) / (sum_i p_i^2)
</i><br/>
where the <i>sum i</i> runs over the particles in the event,
<i>a, b = x, y, z,</i> and <i>p</i> without such an index is 
the absolute size of the three-momentum . This tensor can be 
diagonalized to find eigenvalues and eigenvectors.

<p/>
The above tensor can be generalized by introducing a power 
<i>r</i>, such that
<br/><i>
    S^{ab} = (sum_i p_i^a p_i^b p_i^{r-2}) / (sum_i p_i^r)
</i><br/>
In particular, <i>r = 1</i> gives a linear dependence on momenta 
and thus a collinear safe definition, unlike sphericity.

<p/>
A sphericity analysis object is declared by
<p/><code>class&nbsp; </code><strong> Sphericity sph( power, select) &nbsp;</strong> <br/>
where 
<br/><code>argument</code><strong> power </strong> (<code>default = <strong>2.</strong></code>) :  
is the power <i>r</i> defined above, i.e. 
<br/><code>argumentoption </code><strong> 2.</strong> : gives Spericity, and   
<br/><code>argumentoption </code><strong> 1.</strong> : gives the linear form.  
  
<br/><code>argument</code><strong> select </strong> (<code>default = <strong>2</strong></code>) :  
tells which particles are analyzed,
<br/><code>argumentoption </code><strong> 1</strong> : all final-state particles,  
<br/><code>argumentoption </code><strong> 2</strong> : all observable final-state particles, 
i.e. excluding neutrinos and other particles without strong or
electromagnetic interactions (the <code>isVisible()</code> 
particle method), and
  
<br/><code>argumentoption </code><strong> 3</strong> : only charged final-state particles.  
  
  

<p/>
The analysis is performed by a call to the method
<p/><code>method&nbsp; </code><strong> analyze( event) &nbsp;</strong> <br/>
where 
<br/><code>argument</code><strong> event </strong>  : is an object of the <code>Event</code> class, 
most likely the <code>pythia.event</code> one.
  
<br/>If the routine returns <code>false</code> the analysis failed, 
e.g. if too few particles are present to analyze.
  

<p/>
After the analysis has been performed, a few <code>Sphericity</code> 
class methods are available to return the result of the analysis:
<p/><code>method&nbsp; </code><strong> sphericity() &nbsp;</strong> <br/>
gives the sphericity (or equivalent if <i>r</i> is not 2),
  
<p/><code>method&nbsp; </code><strong> aplanarity() &nbsp;</strong> <br/>
gives the aplanarity (with the same comment),
  
<p/><code>method&nbsp; </code><strong> eigenValue(i) &nbsp;</strong> <br/>
gives one of the three eigenvalues for <i>i</i> = 1, 2 or 3, in 
descending order,
  
<p/><code>method&nbsp; </code><strong> EventAxis(i) &nbsp;</strong> <br/>
gives the matching eigenvector, as a <code>Vec4</code> with vanishing 
time/energy component.
  
<p/><code>method&nbsp; </code><strong> list() &nbsp;</strong> <br/>
provides a listing of the above information.
  
<p/><code>method&nbsp; </code><strong> nError() &nbsp;</strong> <br/>
tells the number of times <code>analyze</code> failed to analyze events.
  

<h3>Thrust</h3>

Thrust is obtained by varying the thrust axis so that the longitudinal
momentum component projected onto it is maximized, and thrust itself is 
then defined as the sum of absolute longitudinal momenta divided by
the sum of absolute momenta. The major axis is found correspondingly 
in the plane transverse to thrust, and the minor one is then defined 
to be transverse to both. Oblateness is the difference between the major 
and the minor values. 

<p/>
The calculation of thrust is more computer-time-intensive than e.g. 
linear sphericity, introduced above, and has no specific advantages except 
historical precedent. In the PYTHIA 6 implementation the search was 
speeded up at the price of then not being guaranteed to hit the absolute
maximum. The current implementation studies all possibilities, but at
the price of being slower, with time consumption for an event with
<i>n</i> particles growing like <i>n^3</i>.

<p/>
A thrust analysis object is declared by
<p/><code>class&nbsp; </code><strong> Thrust thr( select) &nbsp;</strong> <br/>
where 
<br/><code>argument</code><strong> select </strong> (<code>default = <strong>2</strong></code>) :  
tells which particles are analyzed,
<br/><code>argumentoption </code><strong> 1</strong> : all final-state particles,  
<br/><code>argumentoption </code><strong> 2</strong> : all observable final-state particles, 
i.e. excluding neutrinos and other particles without strong or
electromagnetic interactions (the <code>isVisible()</code> 
particle method), and
  
<br/><code>argumentoption </code><strong> 3</strong> : only charged final-state particles.  
  
  

<p/>
The analysis is performed by a call to the method
<p/><code>method&nbsp; </code><strong> analyze( event) &nbsp;</strong> <br/>
where 
<br/><code>argument</code><strong> event </strong>  : is an object of the <code>Event</code> class, 
most likely the <code>pythia.event</code> one.
  
<br/>If the routine returns <code>false</code> the analysis failed, 
e.g. if too few particles are present to analyze.
  

<p/>
After the analysis has been performed, a few <code>Thrust</code> 
class methods are available to return the result of the analysis:
<p/><code>method&nbsp; </code><strong> thrust(), tMajor(), tMinor(), oblateness() &nbsp;</strong> <br/>
gives the thrust, major, minor and oblateness values, respectively, 
<p/><code>method&nbsp; </code><strong> EventAxis(i) &nbsp;</strong> <br/>
gives the matching event-axis vectors, for <i>i</i> = 1, 2 or 3
corresponding to thrust, major or minor, as a <code>Vec4</code> with 
vanishing time/energy component.
  
<p/><code>method&nbsp; </code><strong> list() &nbsp;</strong> <br/>
provides a listing of the above information.
  
<p/><code>method&nbsp; </code><strong> nError() &nbsp;</strong> <br/>
tells the number of times <code>analyze</code> failed to analyze events.
  

<h3>ClusterJet</h3>

<code>ClusterJet</code> (a.k.a. <code>LUCLUS</code> and 
<code>PYCLUS</code>) is a clustering algorithm of the type used for 
analyses of <i>e^+e^-</i> events, see the PYTHIA 6 manual. A few 
options are available for some well-known distance measures. Cutoff 
distances can either be given in terms of a scaled quadratic quantity 
like <i>y = pT^2/E^2</i> or an unscaled linear one like <i>pT</i>. 

<p/>
A cluster-jet analysis object is declared by
<p/><code>class&nbsp; </code><strong> ClusterJet clusterJet( measure, select, massSet,precluster, reassign) &nbsp;</strong> <br/>
where 
<br/><code>argument</code><strong> measure </strong> (<code>default = <strong>Lund</strong></code>) : distance measure, to be provided
as a character string (actually, only the first character is necessary)
<br/><code>argumentoption </code><strong> Lund</strong> : the Lund <i>pT</i> distance,  
<br/><code>argumentoption </code><strong> JADE</strong> : the JADE mass distance, and  
<br/><code>argumentoption </code><strong> Durham</strong> : the Durham <i>kT</i> measure.  
  
<br/><code>argument</code><strong> select </strong> (<code>default = <strong>2</strong></code>) :  
tells which particles are analyzed,
<br/><code>argumentoption </code><strong> 1</strong> : all final-state particles,  
<br/><code>argumentoption </code><strong> 2</strong> : all observable final-state particles, 
i.e. excluding neutrinos and other particles without strong or
electromagnetic interactions (the <code>isVisible()</code> particle 
method), and
  
<br/><code>argumentoption </code><strong> 3</strong> : only charged final-state particles.  
  
<br/><code>argument</code><strong> massSet </strong> (<code>default = <strong>2</strong></code>) : masses assumed for the particles 
used in the analysis
<br/><code>argumentoption </code><strong> 0</strong> : all massless,  
<br/><code>argumentoption </code><strong> 1</strong> : photons are massless while all others are
assigned the <i>pi+-</i> mass, and
  
<br/><code>argumentoption </code><strong> 2</strong> : all given their correct masses.  
  
<br/><code>argument</code><strong> precluster </strong> (<code>default = <strong>false</strong></code>) : 
perform or not a early preclustering step, where nearby particles
are lumped together so as to speed up the subsequent normal clustering.
  
<br/><code>argument</code><strong> reassign </strong> (<code>default = <strong>false</strong></code>) :  
reassign all particles to the nearest jet each time after two jets
have been joined. 
  
  

<p/>
The analysis is performed by a
<p/><code>method&nbsp; </code><strong> analyze( event, yScale, pTscale, nJetMin, nJetMax) &nbsp;</strong> <br/>
where 
<br/><code>argument</code><strong> event </strong>  : is an object of the <code>Event</code> class, 
most likely the <code>pythia.event</code> one.
  
<br/><code>argument</code><strong> yScale </strong>  :  
is the cutoff joining scale, below which jets are joined. Is given
in quadratic dimensionless quantities. Either <code>yScale</code>
or <code>pTscale</code> must be set nonvanishing, and the larger of 
the two dictates the actual value.
  
<br/><code>argument</code><strong> pTscale </strong>  :  
is the cutoff joining scale, below which jets are joined. Is given
in linear quantities, such as <i>pT</i> or <i>m</i> depending on
the measure used, but always in units of GeV. Either <code>yScale</code>
or <code>pTscale</code> must be set nonvanishing, and the larger of 
the two dictates the actual value.
  
<br/><code>argument</code><strong> nJetMin </strong> (<code>default = <strong>1</strong></code>) :  
the minimum number of jets to be reconstructed. If used, it can override 
the <code>yScale</code> and <code>pTscale</code> values. 
  
<br/><code>argument</code><strong> nJetMax </strong> (<code>default = <strong>0</strong></code>) :  
the maximum number of jets to be reconstructed. Is not used if below
<code>nJetMin</code>. If used, it can override the <code>yScale</code>
and <code>pTscale</code> values. Thus e.g. 
<code>nJetMin = nJetMax = 3</code> can be used to reconstruct exactly
3 jets.
  
<br/>If the routine returns <code>false</code> the analysis failed, 
e.g. because the number of particles was smaller than the minimum number
of jets requested.
  

<p/>
After the analysis has been performed, a few <code>ClusterJet</code> 
class methods are available to return the result of the analysis:
<p/><code>method&nbsp; </code><strong> size() &nbsp;</strong> <br/>
gives the number of jets found, with jets numbered 0 through 
<code>size() - 1</code>,
  
<p/><code>method&nbsp; </code><strong> p(i) &nbsp;</strong> <br/>
gives a <code>Vec4</code> corresponding to the four-momentum defined by 
the sum of all the contributing particles to the <i>i</i>'th jet,
  
<p/><code>method&nbsp; </code><strong> jetAssignment(i) &nbsp;</strong> <br/>
gives the index of the jet that the particle <i>i</i> of the event
record belongs to,
  
<p/><code>method&nbsp; </code><strong> list() &nbsp;</strong> <br/>
provides a listing of the reconstructed jets.
  
<p/><code>method&nbsp; </code><strong> nError() &nbsp;</strong> <br/>
tells the number of times <code>analyze</code> failed to analyze events.
  

<h3>CellJet</h3>

<code>CellJet</code> (a.k.a. <code>PYCELL</code>) is a simple cone jet 
finder in the UA1 spirit, see the PYTHIA 6 manual. It works in an 
<i>(eta, phi, eT)</i> space, where <i>eta</i> is pseudorapidity, 
<i>phi</i> azimuthal angle and <i>eT</i> transverse energy.
It will draw cones in <i>R = sqrt(Delta-eta^2 + Delta-phi^2)</i> 
around seed cells. If the total <i>eT</i> inside the cone exceeds 
the threshold, a jet is formed, and the cells are removed from further 
analysis. There are no split or merge procedures, so later-found jets 
may be missing some of the edge regions already used up by previous 
ones.   

<p/>
A cell-jet analysis object is declared by
<p/><code>class&nbsp; </code><strong> CellJet cellJet( etaMax, nEta, nPhi, select, smear, resolution, upperCut, threshold) &nbsp;</strong> <br/>
where 
<br/><code>argument</code><strong> etaMax </strong> (<code>default = <strong>5.</strong></code>) :  
the maximum +-pseudorapidity that the detector is assumed to cover.
  
<br/><code>argument</code><strong> nEta </strong> (<code>default = <strong>50</strong></code>) :  
the number of equal-sized bins that the <i>+-etaMax</i> range 
is assumed to be divided into.
  
<br/><code>argument</code><strong> nPhi </strong> (<code>default = <strong>32</strong></code>) :  
the number of equal-sized bins that the <i>phi</i> range 
<i>+-pi</i> is assumed to be divided into. 
  
<br/><code>argument</code><strong> select </strong> (<code>default = <strong>2</strong></code>) :  
tells which particles are analyzed,
<br/><code>argumentoption </code><strong> 1</strong> : all final-state particles,  
<br/><code>argumentoption </code><strong> 2</strong> : all observable final-state particles, 
i.e. excluding neutrinos and other particles without strong or
electromagnetic interactions (the <code>isVisible()</code> particle 
method), 
and  
<br/><code>argumentoption </code><strong> 3</strong> : only charged final-state particles.  
  
<br/><code>argument</code><strong> smear </strong> (<code>default = <strong>0</strong></code>) : 
strategy to smear the actual <i>eT</i> bin by bin, 
<br/><code>argumentoption </code><strong> 0</strong> : no smearing,  
<br/><code>argumentoption </code><strong> 1</strong> : smear the <i>eT</i> according to a Gaussian 
with width <i>resolution * sqrt(eT)</i>, with the Gaussian truncated 
at 0 and <i>upperCut * eT</i>,  
<br/><code>argumentoption </code><strong> 2</strong> : smear the <i>e = eT * cosh(eta)</i> according 
to a Gaussian with width <i>resolution * sqrt(e)</i>, with the 
Gaussian truncated at 0 and <i>upperCut * e</i>.  
  
<br/><code>argument</code><strong> resolution </strong> (<code>default = <strong>0.5</strong></code>) : 
see above
  
<br/><code>argument</code><strong> upperCut </strong> (<code>default = <strong>2.</strong></code>) : 
see above
  
<br/><code>argument</code><strong> threshold </strong> (<code>default = <strong>0 GeV</strong></code>) : 
completely neglect all bins with an <i>eT &lt; threshold</i>.
  
  

<p/>
The analysis is performed by a
<p/><code>method&nbsp; </code><strong> analyze( event, eTjetMin, coneRadius, eTseed) &nbsp;</strong> <br/>
where 
<br/><code>argument</code><strong> event </strong>  : is an object of the <code>Event</code> class, 
most likely the <code>pythia.event</code> one.
  
<br/><code>argument</code><strong> eTjetMin </strong> (<code>default = <strong>20. GeV</strong></code>) :  
is the minimum transverse energy inside a cone for this to be 
accepted as a jet.
  
<br/><code>argument</code><strong> coneRadius </strong> (<code>default = <strong>0.7</strong></code>) :  
 is the size of the cone in <i>(eta, phi)</i> space drawn around 
the geometric center of the jet.
  
<br/><code>argument</code><strong> eTseed </strong> (<code>default = <strong>1.5 GeV</strong></code>) :  
the mimimum <i>eT</i> in a cell for this to be acceptable as 
the trial center of a jet. 
  
<br/>If the routine returns <code>false</code> the analysis failed, 
but currently this is not foreseen ever to happen.
  

<p/>
After the analysis has been performed, a few <code>CellJet</code> 
class methods are available to return the result of the analysis:
<p/><code>method&nbsp; </code><strong> size() &nbsp;</strong> <br/>
gives the number of jets found, with jets numbered 0 through 
<code>size() - 1</code>,
  
<p/><code>method&nbsp; </code><strong> eT(i) &nbsp;</strong> <br/>
gives the <i>eT</i> of the <i>i</i>'th jet, where jets have been
ordered with decreasing <i>eT</i> values,
  
<p/><code>method&nbsp; </code><strong> etaCenter(i), phiCenter(i) &nbsp;</strong> <br/>
gives the <i>eta</i> and <i>phi</i> coordinates of the geometrical 
center of the <i>i</i>'th jet,
  
<p/><code>method&nbsp; </code><strong> etaWeighted(i), phiWeighted(i) &nbsp;</strong> <br/>
gives the <i>eta</i> and <i>phi</i> coordinates of the 
<i>eT</i>-weighted center of the <i>i</i>'th jet,
  
<p/><code>method&nbsp; </code><strong> multiplicity(i) &nbsp;</strong> <br/>
gives the number of particles clustered into the <i>i</i>'th jet,
  
<p/><code>method&nbsp; </code><strong> pMassless(i) &nbsp;</strong> <br/>
gives a Vec4 corresponding to the four-momentum defined by the 
<i>eT</i> and the weighted center of the <i>i</i>'th jet,
  
<p/><code>method&nbsp; </code><strong> pMassive(i) &nbsp;</strong> <br/>
gives a <code>Vec4</code> corresponding to the four-momentum defined by 
the sum of all the contributing cells to the <i>i</i>'th jet, where 
each cell contributes a four-momentum as if all the <i>eT</i> is 
deposited in the center of the cell,
  
<p/><code>method&nbsp; </code><strong> m(i) &nbsp;</strong> <br/>
gives the invariant mass of the <i>i</i>'th jet, defined by the 
<code>pMassive</code> above,
  
<p/><code>method&nbsp; </code><strong> list() &nbsp;</strong> <br/>
provides a listing of the above information (except <code>pMassless</code>, 
for reasons of space).
  
<p/><code>method&nbsp; </code><strong> nError() &nbsp;</strong> <br/>
tells the number of times <code>analyze</code> failed to analyze events.
  

</body>
</html>

<!-- Copyright (C) 2008 Torbjorn Sjostrand -->
