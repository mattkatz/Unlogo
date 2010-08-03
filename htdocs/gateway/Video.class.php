<?php

class Upload {

	var $filename;

	function Upload() {
		global $_SERVER;
		
		$arrIp = explode('.', $_SERVER['REMOTE_ADDR']);
		list($usec, $sec) = explode(' ', microtime());
		$usec = (integer) ($usec * 65536);
		$sec = ((integer) $sec) & 0xFFFF;
		$strUid = sprintf("%08x-%04x-%04x", ($arrIp[0] << 24) | ($arrIp[1] << 16) | ($arrIp[2] << 8) | $arrIp[3], $sec, $usec);
		$this->filename = $strUid.".mov";
	}
	
	function doUpload($index, $destination) {
		global $_FILES;
			
		if(!isset($_FILES[$inde])) {
			finish("no file was posted");
		}
	
		if (!move_uploaded_file($_FILES['image']['tmp_name'], $uploadfile)) {
			finish("couldn't move uploaded file. possible file upload attack!");
		}
		
		
		// Ty to open the template directory.
		if (!($handle = opendir("templates"))) {
			finish("couldn't open template directory.");
		}
	}
	
	
	function logUpload() {
		$sql="INSERT INTO uploads SET 
			filename='$target_path', 
			device_token='$device_token', 
			date_uploaded=NOW(), 
			files_dump='$files_dump', 
			request_dump='$request_dump'";
		mysql_query($sql);
		
		if(mysql_errno()>0) {
			print mysql_error();
		} else {
			print "ok";	
		}
	}
	
}

?>