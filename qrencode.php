<?php
$br = (php_sapi_name() == "cli")? "":"<br>";

if(!extension_loaded('qrencode')) {
	dl('qrencode.' . PHP_SHLIB_SUFFIX);
}
$t1 = microtime(true);
// echo qrcode_version()."\n";
// echo gd_version()."\n";

$resource = qrencode_create("http://www.163.com", QRENCODE_QRECLEVEL_Q, QRENCODE_MODE_8);
//echo "resource: " . $resource ."\n";
if (!is_null($resource)) {
    echo qrencode_save($resource, "/Users/kentchen/Downloads/t7.png", 255) . "\n";
}

//echo qrencode_version()."\n";
$t2 = microtime(true);

echo (($t2-$t1)*1000).":ms\n";
?>
