<?php

class MediaFile {

	var $filename;  // The local (server) filename for the media

	var $media_id;  // The system-wide unique identifier for the media

	var $errors = array();

	var $media_type = NULL;

	var $valid_media_types = array("image", "video");

	var $user_id;

	var $status;
	
	var $valid_statuses = array("new", "queued", "processing", "done");
	
	var $in_database = false;
	
	var $title="";
	
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
		
		$sql="SELECT filename, title, media_id, media_type, user_id, status, date_modified, date_uploaded 
			FROM media WHERE id={$this->id} LIMIT 1";
		$result = mysql_query($sql);
		if(mysql_errno() > 0) {
			$this->errors[] = "Couldn't load media id={$this->id}";
			return false;
		}
		$row = mysql_fetch_assoc($result);
		$this->title = $row['title'];
		$this->media_id = $row['media_id'];
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
		if(!copy("../uploads/{$this->filename}", "../processed/{$this->filename}")) {
			$this->errors[] = "There was an error processing the file.";
			return false;	
		}
		$this->status = 'done';
		if(!$this->save()) {
			return false;	
		}
		return true;
	}
	
	function getOwner() {
		if(isset( $this->user_id )) {
			return new User( $this->user_id );
		} else {
			return false;
		}
	}
	
	/*
	function generateMediaID() {
		$arrIp = explode('.', $_SERVER['REMOTE_ADDR']);
		list($usec, $sec) = explode(' ', microtime());
		$usec = (integer) ($usec * 65536);
		$sec = ((integer) $sec) & 0xFFFF;
		$strUid = sprintf("%08x-%04x-%04x", ($arrIp[0] << 24) | ($arrIp[1] << 16) | ($arrIp[2] << 8) | $arrIp[3], $sec, $usec);
		return $strUid;
	}
	*/
	
	function doUpload($files_index="file", $destination="uploads/") {
		global $_FILES, $_SERVER, $_REQUEST;
	
		if($destination[strlen($destination)-1] != "/")
			$destination .= "/";
			
		if(!isset($_FILES[$files_index])) {
			$this->errors[] = "No file was posted.";
			return false;
		}
	
		// Ty to open the upload directory.
		if (!($handle = opendir($destination))) {
			$this->errors[] = "couldn't open upload directory.";
			return false;
		}
		
		if(empty($_REQUEST['media_id'])) {
			$this->errors[] = "No media ID was provided.";
			return false;
		}
		
		$this->title = $_REQUEST['title'];
		$this->media_id = $_REQUEST['media_id'];
		$sql="SELECT id FROM media WHERE media_id='{$this->media_id}' ";
		$result = mysql_query($sql);
		if(mysql_num_rows($result)>0) {
			$this->errors[] = "The media ID {$this->media_id} already exists.";
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
			
			$this->filename = $this->media_id.$ext;
			$this->media_type = "image";
		} else if(eregi('video', $_FILES[$files_index]['type'])) {
			$this->media_type = "video";
			$this->filename = $this->media_id.".mov";
		} else {
			$this->errors[] = "The uploaded file is not an image or video. Please upload a valid file!";
			return false;
		}
		
		$new_path = $destination.$this->filename;
		if(file_exists($new_path)) {
			$this->errors[] = "This file has already been uploaded.";
			return false;	
		}
		
		if (!move_uploaded_file($_FILES[$files_index]['tmp_name'], $new_path)) {
			$this->errors[] = "couldn't move uploaded file. possible file upload attack!";
			return false;
		}
	
		return true;
	}
	
	function save() {
		global $_FILES, $_REQUEST;
	
		$file = basename($this->filename);
		$file = mysql_escape_string($file);
		$title = mysql_escape_string($this->title);
		
		if($this->in_database) {
			$sql="UPDATE media SET 
				filename = '$file',
				title = '$title',
				media_id = '{$this->media_id}',
				media_type='{$this->media_type}',
				user_id = $this->user_id,
				status = '$this->status'
				WHERE id={$this->id} LIMIT 1";
		} else {
			$sql="INSERT INTO media SET 
				filename='$file', 
				title = '$title',
				media_id = '{$this->media_id}',
				media_type='{$this->media_type}',
				date_uploaded=NOW(), 
				user_id = $this->user_id,
				status='new'";
			$this->in_database = true;
		}
		
		mysql_query($sql);
		if(mysql_errno()>0) {
			$this->errors[] =  mysql_error();
			return false;
		} else {
			$this->id = mysql_insert_id();
			return true;
		}
	}
}	
?>