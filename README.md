# php-qrencode

基于libqrencode+libgd的生成二维码的php扩展


## 安装:

1，安装libpng & libjpg

	sudo apt-get install libpng-dev
	sudo apt-get install libjpg-dev

2，安装libqrencode

	wget http://fukuchi.org/works/qrencode/qrencode-3.4.4.tar.gz
	tar zxvf qrencode-3.4.4.tar.gz
	cd qrencode-3.4.4/
	./configure
	make&make install

3，安装libgd

	sudo apt-get install libgd-dev

4，安装php-qrencode

	git clone https://github.com/Leon2012/php-qrencode
	cd php-qrencode
	phpize
	./configure
	make&sudo make install

5，修改配置文件

	vi php.ini
	extension=qrencode.so

## 用法:
	<?php
	$resource = qrencode_create("test");
	qrencode_save($resource, "/Users/kentchen/Downloads/t3.png");//保存文件
	echo qrencode_version()."\n";//查看版本号
	?>
	
## 和phpqrcode对比

php-qrencode代码:

	<?php
	$t1 = microtime(true);
	$qrcode_image = "/Users/kentchen/Downloads/qrencode1.png";
	$content = "hello world";
	$resource = qrencode_create($content, QRENCODE_QRECLEVEL_Q, QRENCODE_MODE_8);
	qrencode_save($resource, $qrcode_image);
	$t2 = microtime(true);
	echo (($t2-$t1)*1000).':ms';
	?>
	
运行时间:0.60701370239258:ms
	
phpqrcode代码:

	<?php
	include('./phpqrcode/phpqrcode.php'); 
	$t1 = microtime(true);
	$qrcode_image = "/Users/kentchen/Downloads/qrencode2.png";
	$content = "hello world";
	QRcode::png($content, $qrcode_image, QR_ECLEVEL_Q, 12);
	$t2 = microtime(true);
	echo (($t2-$t1)*1000).':ms';
	?>
运行时间:23.189067840576:ms

