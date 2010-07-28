<?php

class AllMetaAlerts extends TPage
{
  public function __construct()
  {
    parent::__construct();
    $this->data_=CSQLMap::get()->queryForList('SelectMetaAlertsSummary');
    // add link field
    foreach($this->data_ as $e)
    {
      $url    =$this->Service->constructUrl( 'MetaAlert', array('id' => $e->id) );
      $href   ="<a href=\"$url\">see</a>";
      $e->link=$href;
    }
  }

  public function onLoad()
  {
    // initialization of GridData
    if(!$this->IsPostBack)
    {
      $this->DataGrid->DataSource=$this->data_;
      $this->DataGrid->dataBind();
    }
  }

  public function changePage($sender, $param)
  {
    $this->DataGrid->CurrentPageIndex=$param->NewPageIndex;
    $this->DataGrid->DataSource      =$this->data_;
    $this->DataGrid->dataBind();
  }

  public function pagerCreated($sender, $param)
  {
    $param->Pager->Controls->insertAt(0, 'Page: ');
  }

  private $data_;
}

?>