<?php

class User {

	var $id;

	var $udid;

	var $device_token;
	
	var $device_name;

	var $in_database = false;

	function User($id=NULL) {
		if($id!=NULL) {
			$this->id = intval($id);
			$this->load();
		}
	}

	function load() {
		if($this->id==NULL) {
			return false;	
		}
		$sql="SELECT id, udid, device_token, date_added, device_name FROM users WHERE id={$this->id} LIMIT 1";
		$result = mysql_query($sql);
		if(mysql_num_rows($result) > 0) {
			$this->in_database = true;
			$row = mysql_fetch_assoc($result);
			$this->id = $row['id'];
			$this->udid = $row['udid'];
			$this->device_token = $row['device_token'];
			$this->device_name = $row['device_name'];
		}	
	}

	function loadFromUDID($udid) {
		$this->udid = $udid;
		$sql="SELECT id, udid, device_token, date_added, device_name FROM users WHERE udid='$udid' LIMIT 1";
		$result = mysql_query($sql);
		if(mysql_num_rows($result) > 0) {
			$this->in_database = true;
			$row = mysql_fetch_assoc($result);
			$this->id = $row['id'];
			$this->udid = $row['udid'];
			$this->device_token = $row['device_token'];
			$this->device_name = $row['device_name'];
			return true;
		} else {
			$this->in_database = false;	
			return false;
		}
	}

	function getMedia() {
		if($this->id==NULL) {
			return false;	
		}
		
		$media = array();
		
		$sql="SELECT id FROM media WHERE user_id={$this->id}";	
		$result = mysql_query($sql);
		if(mysql_num_rows($result) > 0) {
			while($row = mysql_fetch_assoc($result)) {
				$media[] = new MediaFile($row['id']);	
			}
		}
		return $media;
	}

	function getFinishedMedia() {
		if($this->id==NULL) {
			return false;	
		}
		$sql="SELECT id FROM uploads WHERE status='done' AND user_id={$this->id}";	
		$result = mysql_query($sql);
		if(mysql_num_rows($result) > 0) {
			$media = array();
			while($row = mysql_fetch_assoc($result)) {
				$media[] = new MediaFile($row['id']);	
			}
			return $media;
		} else {
			return false;	
		}
	}

	function save() {
		$device_token = mysql_escape_string($this->device_token);
		$device_name = mysql_escape_string($this->device_name);
			
		
		if($this->in_database) {
			$sql="UPDATE users SET device_token='{$device_token}', 
				device_name='{$device_name}' 
				WHERE id={$this->id} LIMIT 1";
			mysql_query($sql);
			if(mysql_errno() > 0) {
				$this->errors[] = $sql;
				$this->errors[] = mysql_error();
				return false;	
			} else {
				return true;
			}
		} else {
			$sql="INSERT INTO users SET 
				udid = '{$this->udid}',
				device_token = '{$device_token}',
				device_name = '{$device_name}',
				date_added=NOW()";
			mysql_query($sql);
			if(mysql_errno() > 0) {
				$this->errors[] = mysql_error();
				return false;	
			} else {
				$this->id = mysql_insert_id();
				$this->in_database = true;
				return true;	
			}
		}
	}
	
	function readDeviceTokenFromFile($filename) {
		if(!file_exists($filename) || !is_readable($filename) ) {
			$this->errors[] = "$filename does not exist or is not readable.";
			return false;	
		}
		$handle = fopen($filename, "rb");
		$this->device_token = fread($handle, filesize($filename));
		fclose($handle);
		return true;
	}
}

?>