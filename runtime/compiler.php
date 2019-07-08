<?php
$script = file_get_contents("./test.ekwa");
$lines = explode("\n", $script);
$filename = "./instructions";
$bytecode = NULL;

$tokens = array(
	"EKWA_VAR"	=> "\x01",
	"EKWA_BUFF"	=> "\x02",
	"EKWA_ARG"	=> "\x03",
	"EKWA_CALL"	=> "\x04",
	"EKWA_JMP"	=> "\x05",
	"EKWA_FSET"	=> "\x06",
	"EKWA_WRT"	=> "\x07",
	"EKWA_CMP"	=> "\x08",
	"EKWA_IFS"	=> "\x0a",
	"EKWA_IFB"	=> "\x0b",
	"EKWA_INFO"	=> "\x0c",
	"EKWA_SHOW"	=> "\x0d",
	"EKWA_RMV"	=> "\x0e",
	"EKWA_VAL"	=> "\x0f",
	"EKWA_END"	=> "\x10"
);

$types = array(
	"EKWA_BYTES"	=> "\x00",
	"EKWA_INT"		=> "\x01",
	"EKWA_FLOAT"	=> "\x02",
	"EKWA_CUSTOM"	=> "\x03"
);

$vars = array();

foreach ($lines as $line) {
	$elements = explode("\t", $line);

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

	if (isset($types[$elements[2]])
		&& $elements[0] == "EKWA_VAR") {
		$bytecode .= "\x00\x01";
		$bytecode .= $types[$elements[2]];
		$vars[$elements[1]] = $elements[2];
		continue;
	}

	if ($elements[0] == "EKWA_VAL") {
		if ($vars[$elements[1]] == "EKWA_INT") {
			$d_bytes = pack('i', (int)$elements[2]);
			$length = strlen($d_bytes);
			$bytecode .= pack('n', $length);
			$bytecode .= $d_bytes;
			continue;
		}
		if ($vars[$elements[1]] == "EKWA_FLOAT") {
			$d_bytes = pack('f', (float)$elements[2]);
			$length = strlen($d_bytes);
			$bytecode .= pack('n', $length);
			$bytecode .= $d_bytes;
			continue;
		}
	}

	$length = strlen($elements[2]);
	$bytecode .= pack('n', $length);
	$bytecode .= (string)$elements[2];
}

$fp = fopen($filename, "wb");
fwrite($fp, $bytecode);
fclose($fp);

