<?php

class MediaFile {

	var $filename;

	var $new_filename;

	var $errors = array();

	var $media_type = NULL;

	var $user_id;

	var $status;
	
	var $in_database = false;
	
	function MediaFile($id=NULL) {
		if($id!=NULL) {
			$this->id = intval($id);
			$this->in_database = $this->load();	
		}
	}
	
	function load() {
		if(!isset($this->id)) {
			return false;
		}
		
		$sql="SELECT filename, media_type, user_id, status, date_modified, date_uploaded 
			FROM uploads WHERE id={$this->id} LIMIT 1";
		$result = mysql_query($sql);
		if(mysql_errno() > 0) {
			$this->errors[] = "Couldn't load media id={$this->id}";
			return false;
		}
		$row = mysql_fetch_assoc($result);
		$this->filename = $row['filename'];
		$this->media_type = $row['media_type'];
		$this->user_id = $row['user_id'];
		$this->status = $row['status'];
		$this->date_uploaded = $row['date_uploaded'];
		return true;
	}
	
	
	function process() {
		// This is where the unlogo exe will be called. 
		// For now, just make a copy
		copy("../uploads/{$this->filename}", "../processed/{$this->filename}");
		$this->status = 'done';
		$this->save();
	}
	
	function getOwner() {
		if(isset( $this->user_id )) {
			return new User( $this->user_id );
		} else {
			return false;
		}
	}
	
	
	function generateUniqueName($ext) {
		$arrIp = explode('.', $_SERVER['REMOTE_ADDR']);
		list($usec, $sec) = explode(' ', microtime());
		$usec = (integer) ($usec * 65536);
		$sec = ((integer) $sec) & 0xFFFF;
		$strUid = sprintf("%08x-%04x-%04x", ($arrIp[0] << 24) | ($arrIp[1] << 16) | ($arrIp[2] << 8) | $arrIp[3], $sec, $usec);
		return $strUid.$ext;
	}
	
	
	function doUpload($files_index="file", $destination="uploads/") {
		global $_FILES, $_SERVER, $_REQUEST;
	
		if($destination[strlen($destination)-1] != "/")
			$destination .= "/";
			
		if(!isset($_FILES[$files_index])) {
			$this->errors[] = "No file was posted.";
			return false;
		}
	
		// Ty to open the template directory.
		if (!($handle = opendir($destination))) {
			$this->errors[] = "couldn't open template directory.";
			return false;
		}
	
		if(eregi('image', $_FILES[$files_index]['type'])) {
			
			if(eregi("jpeg", $_FILES[$files_index]['type'])) {
				$ext = ".jpg";
			} else if(eregi("png", $_FILES[$files_index]['type'])) {
				$ext = ".png";
			} else {
				$this->errors[] = "Image not recognized.  Please upload a jpeg or png image.";
				return false;
			}
			
			$this->filename = MediaFile::generateUniqueName($ext);
			$this->media_type = "image";
		} else if(eregi('video', $_FILES[$files_index]['type'])) {
			$this->media_type = "video";
			$this->filename = MediaFile::generateUniqueName(".mov");
		} else {
			$this->errors[] = "The uploaded file is not an image or video. Please upload a valid file!";
			return false;
		}
	
		if (!move_uploaded_file($_FILES[$files_index]['tmp_name'], $destination.$this->filename)) {
			$this->errors[] = "couldn't move uploaded file. possible file upload attack!";
			return false;
		}
	
		return true;
	}
	
	function save() {
		global $_FILES, $_REQUEST;
	
		$file = basename($this->filename);
		$file = mysql_escape_string($file);
		
		if($this->in_database) {
			$sql="UPDATE uploads SET 
				filename = '$file',
				media_type='{$this->media_type}',
				user_id = $this->user_id,
				status = '$this->status'
				WHERE id={$this->id} LIMIT 1";
		} else {
			$sql="INSERT INTO uploads SET 
				filename='$file', 
				media_type='{$this->media_type}',
				date_uploaded=NOW(), 
				user_id = $this->user_id,
				status='inqueue'";
			$this->in_database = true;
		}
		
		mysql_query($sql);
		if(mysql_errno()>0) {
			$this->errors[] =  mysql_error();
			return false;
		} else {
			return true;
		}
	}
}	
?>