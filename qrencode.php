<?php
$br = (php_sapi_name() == "cli")? "":"<br>";

if(!extension_loaded('qrencode')) {
	dl('qrencode.' . PHP_SHLIB_SUFFIX);
}

// echo qrcode_version()."\n";
// echo gd_version()."\n";

$resource = qrencode_create("http://www.163.com");
echo "resource: " . $resource ."\n";

echo qrencode_save($resource, "/Users/kentchen/Downloads/t3.png") . "\n";


echo qrencode_version()."\n";
?>
