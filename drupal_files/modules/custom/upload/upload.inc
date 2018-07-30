<?php

/**
 * Implements hook_form()
 */
function upload_document($form, &$form_state){

  $form = array();

  // File upload tag to browse the file
  $form['upload_item'] = array(
    '#type' => 'file',
    '#attributes' => array(
      'multiple' => array(
        'multiple',
      ),
    ),
    '#title' => t('Please, select your document'),
    '#description' => '<em>(Allowed formats are: PDF, doc, docx</em>)',
  );

  // Optional note entered by the user
  $form['note'] = array(
    '#type' => 'textarea',
    '#title' => t('Notes:'),
  );

  // Submit button to submit file upload request
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Upload Document'),
    '#submit' => array('upload_submit')
  );

  return $form;
}


/**
 * Handling storing the uploaded file inside the uploads directory
 * as well as its records inside the document database table
*/
function upload_submit($form, &$form_state){
  // Getting the current user information
  global $user;

  // User note received from note textarea form field
  $note = $form_state['values']['note'];

  // Acceptable document extensions
  $extensions = 'pdf doc docx';
  $validators = array(
    'file_validate_size' => array(
      10*1024*1024              // Acceptable document size 10MB
    ),
    'file_validate_extensions' => array($extensions),
  );

  // Begin building file object.
  $file = new stdClass();
  $file->filename = trim(
    drupal_basename($_FILES['files']['name']['upload_item']), '.');

  // Renaming the file with a unique id to avoid writing the already
  // existing file
  $ext = pathinfo($file->filename, PATHINFO_EXTENSION);
  $file->uniquename = uniqid().".$ext";
  $_FILES['files']['name']['upload_item'] = $file->uniquename;
  $file->size = $_FILES['files']['size']['upload_item'];

  // Document sotrage in private location
  $location = 'private://';

  // Storing the document data inside the server
  $did = db_insert('document')
  ->fields(array(
    'title' => $file->filename,
    'type' => $ext,
    'size' => $file->size,
    'path' => $file->uniquename,
    'note' => $note,
    'uid' => $user->uid,
    'created' => REQUEST_TIME,
  ))
  ->execute();


  // Storing the data on the server private directory
  $upload_check = file_save_upload(
    'upload_item', $validators, $location);


  if($upload_check){
    drupal_set_message(
      "<b>$file->filename</b> has been successfully uploaded");
  }
}
