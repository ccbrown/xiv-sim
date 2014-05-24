<?php
if (isset($_GET['wdmg'], $_GET['wdel'], $_GET['str'], $_GET['crit'], $_GET['ss'], $_GET['det'], $_GET['len'])) {
	$wdmg = intval($_GET['wdmg']);
	$wdel = floatval($_GET['wdel']);
	$str = intval($_GET['str']);
	$crit = intval($_GET['crit']);
	$ss = intval($_GET['ss']);
	$det = intval($_GET['det']);
	$len = intval($_GET['len']);

	if (true 
		&& $wdmg >= 1 && $wdm <= 10000
		&& $wdel > 0.0 && $wdel <= 100
		&& $str >= 1 && $str <= 10000
		&& $crit >= 1 && $crit <= 10000
		&& $det >= 1 && $det <= 10000
		&& $len >= 1 && $len <= 30 * 60
	) {
		passthru("./simulator single-html 'WDMG=$wdmg WDEL=$wdel STR=$str CRIT=$crit SS=$ss DET=$det LEN=$len'");
		exit;
	}
}
?>

<center>

<br /><br /><br /><br /><br />

<form action="" method="get">
	<table>
	<tr><td>Weapon Damage</td><td><input type="text" name="wdmg" /></td></tr>
	<tr><td>Weapon Delay</td><td><input type="text" name="wdel" /></td></tr>
	<tr><td>Strength</td><td><input type="text" name="str" /></td></tr>
	<tr><td>Critical Hit Rate</td><td><input type="text" name="crit" /></td></tr>
	<tr><td>Skill Speed</td><td><input type="text" name="ss" /></td></tr>
	<tr><td>Determination</td><td><input type="text" name="det" /></td></tr>
	<tr><td>Simulation Length (Seconds)</td><td><input type="text" name="len" /></td></tr>
	</table>
	<input type="submit" value="Submit" />
</form>

<br /><br />

<small><a href="https://github.com/ccbrown/xiv-sim" target="_blank">https://github.com/ccbrown/xiv-sim</a></small>

</center>