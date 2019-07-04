<?php
$script = file_get_contents("./test.ekwa");
$lines = explode("\n", $script);
$filename = "./instructions";
$bytecode = NULL;

$tokens = array(
	"EKWA_VAR"	=> "\x01",
	"EKWA_BUFF" => "\x02",
	"EKWA_ARG"	=> "\x03",
	"EKWA_CALL"	=> "\x04",
	"EKWA_JMP"	=> "\x05",
	"EKWA_FSET" => "\x06",
	"EKWA_WRT"	=> "\x07",
	"EKWA_CMP"	=> "\x08",
	"EKWA_IFS"	=> "\x0a",
	"EKWA_IFB"	=> "\x0b",
	"EKWA_INFO" => "\x0c",
	"EKWA_SHOW" => "\x0d",
	"EKWA_RMV"	=> "\x0e",
	"EKWA_END"	=> "\x0f"
);

foreach ($lines as $line) {
	$elements = explode(" ", $line);

	if (!isset($tokens[$elements[0]])) {
		continue;
	}

	$bytecode .= $tokens[$elements[0]];

	if (!isset($elements[1])) {
		$bytecode .= "\x00\x00";
		continue;
	}

	$length = strlen($elements[1]);
	$bytecode .= pack('n', $length);
	$bytecode .= $elements[1];

	if (!isset($elements[2])) {
		$bytecode .= "\x00\x00";
		continue;
	}

	$length = strlen($elements[2]);
	$bytecode .= pack('n', $length);
	$bytecode .= $elements[2];
}

$fp = fopen($filename, "wb");
fwrite($fp, $bytecode);
fclose($fp);
