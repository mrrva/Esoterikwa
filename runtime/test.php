<?php
$filename = "./instructions";
$command = "make && ./program";

$instructions = "\x0c\x00\x00\x0c\x00\x00\x0c\x00\x00\x0d\x00\x00";
$fp = fopen($filename, "wb");
fwrite($fp, $instructions);
fclose($fp);

//echo system($command);
