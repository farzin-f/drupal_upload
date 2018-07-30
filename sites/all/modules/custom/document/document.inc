<?php

/**
 * Getting the list of all documents uploaded by the current user
 * And display them as a list
 */
function document_all(){

  // Getting the current user information
  global $user;

  // Get the list of all documents using the user id
  $documents = db_select('document', 'd')
    ->fields('d')
    ->condition('uid', $user->uid,'=')
    ->execute();

  // If any record exists
  if($documents->rowCount() > 0 ){
    // Creating the rows to be displayed in a table using the table
    // theme
    $rows = array();
    $counter = 1;
    foreach ($documents as $d) {
      // Getting the document information and add them to the
      // table rows
      // $file_uri = file_create_url(file_build_uri($d->path));

      $file_uri = file_create_url("document/$d->did");
      $title = "<a href=$file_uri>$d->title</a>";
      $size = number_format($d->size/1024, 2, '.',',');
      $uploaded_at = date("F j, Y, g:i a", $d->created);

      // Table rows
      $rows[] = array('# '.$counter, $title, $size, $uploaded_at);
      ++$counter;
    }

    // Table header
    $headers = array("No","Title", "Size (KB)", 'Uploaded at');
    // The table output
    $output = theme('table', array(
                'header' => $headers,
                'rows' => $rows)) . theme('pager');

  } else {
    $output = "<div class='alert alert-info' role='alert'><h4 class='alert-heading'><span class='glyphicon glyphicon-exclamation-sign' aria-hidden='true'></span>
  <span class='sr-only'>Error:</span>No Documents</h4><p>No documents found in your storage.</p>
  <hr>
  <p class='mb-0'> Please upload your documents using <em>Upload Document</em> tab</p></div>";

  }

  return $output;
}


/**
 * View the selected document
 */
function document_show($did){
  // Get the document by its id
  $result = db_select('document', 'd')
    ->fields('d')
    ->condition('did', $did)
    ->execute();


  // Displaying the document and some information
  // through the output
  foreach ($result as $record) {
    // Add the document title to the page output

    $output = "<div class=row>";
    $output .= "<div class=col-md-4>";
    $output .= "<label>Title:</label><p>$record->title</p>";
    if(!empty($record->note)){
      $output .= "<label>Notes:</label><p>$record->note</p>";
    }
    // Delete button
    $output .= "<hr>";
    $delete_uri = file_create_url("document/$record->did/delete");
    $output .= "<a href=$delete_uri class='btn btn-danger icon-before'><span class='glyphicon glyphicon-trash'></span>Delete</a>";

    $output .= "</div>";        //end of column

    // Create the document uri
    $file_uri = file_create_url("private://$record->path");
    // Display the pdf document using the HTML5 pdf
    $output .= "<div class=col-md-8><embed src=$file_uri width=100% height=400 type='application/pdf'></div>";

    $output .= "</div>";        //end of row

  }

  return $output;

}

// Delete the record of the document from the database
// and its file in the directory
function document_delete($did){

  // Get the document record
  $result = db_select('document', 'd')
    ->fields('d')
    ->condition('did', $did)
    ->execute();


  // Delete the document from storage
  foreach($result as $record){
    $doc_title = $record->title;

    $file_path = drupal_realpath("private://$record->path");
    drupal_unlink($file_path);
  }


  // Delete the document from the database
  db_delete('document')
    ->condition('did', $did)
    ->execute();


  // Display the deleted message and redirect to document list
  // page
  drupal_set_message("<b>$doc_title</b> has been deleted.", 'error');
  drupal_goto('document');

}
