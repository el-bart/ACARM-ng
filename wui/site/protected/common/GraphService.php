<?php

// TODO: 'jpgraph' should be moved somewhere to protected/ directory, where all the sources are (access to this dir is protected with .htaccess)
require_once('lib/jpgraph/jpgraph.php');
require_once('lib/jpgraph/jpgraph_bar.php');
require_once('lib/jpgraph/jpgraph_log.php');
require_once('lib/jpgraph/jpgraph_led.php');
require_once('lib/jpgraph/jpgraph_pie.php');
require_once('lib/jpgraph/jpgraph_pie3d.php');
require_once('lib/jpgraph/jpgraph_line.php');
require_once('lib/jpgraph/jpgraph_date.php');

class GraphParams
{
  function __construct($request)
  {
    $this->type  = TPropertyValue::ensureString($this->getRequestOrDefault($request, 'graph', 'You must specify the type of the graph'));
    $this->width = TPropertyValue::ensureInteger($this->getRequestOrThrow($request, 'width',  'You must specify the width of the graph'));
    $this->height= TPropertyValue::ensureInteger($this->getRequestOrThrow($request, 'height', 'You must specify the height of the graph'));
    $this->title = TPropertyValue::ensureString($this->getRequestOrDefault($request, 'title', 'No title'));
    $this->query = TPropertyValue::ensureString($this->getRequestOrDefault($request, 'query', null));

    $qparam=new CParamRange();

    $this->qparam->src=$this->getRequestOrDefault($request, 'src', null);
    $this->qparam->dst=$this->getRequestOrDefault($request, 'dst', null);

    if ($this->qparam->dst=='any')
      {
        $this->qparam->dst='0.0.0.0';
        $this->qparam->ignoredst=1;
      } else
      $this->qparam->ignoredst=0;

    if ($this->qparam->src=='any')
      {
        $this->qparam->src='0.0.0.0';
        $this->qparam->ignoresrc=1;
      } else
      $this->qparam->ignoresrc=0;

    $this->qparam->date_from=$this->getRequestOrDefault($request, 'from', null);
    $this->qparam->date_to=$this->getRequestOrDefault($request, 'to', null);
    $this->qparam->severities=$this->getRequestOrDefault($request, 'severities', null);
  }

  private function getRequestOrDefault($request, $field, $value)
  {
    if ($request->contains($field))
      return $request[$field];
    return $value;
  }

  private function getRequestOrThrow($request, $field, $text)
  {
    if (!$request->contains($field))
      throw new TConfigurationException($text);
    return $request[$field];
  }

  public $type;  //type of the graph
  public $width;
  public $height;
  public $title;
  public $query; //DB query name
  public $qparam; //parameters for queries
}


class GraphService extends TService
{
  public function init($config)
  {
    $this->Response->ContentType="image/png";
    $request = Prado::getApplication()->getRequest();
    $this->params=new GraphParams($request);
  }

  public function getWidth()
  {
    return $this->width;
  }

  public function setWidth($value)
  {
    $this->width = TPropertyValue::ensureInteger($value);
  }

  public function getHeight()
  {
    return $this->height;
  }

  public function setHeight($value)
  {
    $this->height = TPropertyValue::ensureInteger($value);
  }

  public function run()
  {
    switch( $this->params->type )
    {
    case "AlertTimeSeries":
      $graph = $this->createAlertTimeSeries();
      break;
    case "SeverityPie":
      $graph = $this->createSeverityChart();
      break;
    case "AlertTypes":
      $graph = $this->createAlertTypeChart();
      break;
    case "MetaAlertTimeSeries":
      $graph = $this->createAlertTimeSeries();
      break;
    default:
      // Display LED screen with an error message in place of a graph
      $led = new DigitalLED74(4);
      $led->SetSupersampling(5);
      $led->StrokeNumber(' WRONG GRAPH TYPE ',LEDC_YELLOW);
      return;
    }
    $graph->Stroke();
  }

  private function issueQuery2d($param,$append="")
  {
    if ($param->query->qparam === null)
      $pairs=CSQLMap::get()->queryForList($param->query,$param->qparam);
    else
      $pairs=CSQLMap::get()->queryForList($param->query,$param->qparam);

    foreach( $pairs as $e )
    {
      $xdata[] = trim($e->key).$append;
      $ydata[] = $e->value;
    }

    return array($xdata, $ydata);
  }

  private function issueQuery2dTime($param)
  {
    $pairs=CSQLMap::get()->queryForList($param->query,$param->qparam);

    foreach( $pairs as $e )
    {
      $xdata[] = strtotime($e->key);
      $ydata[] = ($e->value===null)?0:$e->value;
    }

    return array($xdata, $ydata, $param->qparam->severities);
  }

  private function severityToColor($sev)
  {
    $name=substr($sev,0,4); //strip "(%d)"

    switch ($name)
      {
      case 'prob':
        return "#ff8800";
      case 'warn':
        return "#ffee00";
      case 'erro':
        return "#ff5500";
      case 'crit':
        return "#ff0000";
      case 'info':
        return "#00ff00";
      case 'debu':
        return "#2d88ff";
      }
    return "#cccccc";
  }

  private function createSeverityChart()
  {
    $data=$this->issueQuery2d($this->params," (%d)");

    if (count($data[0])==0)
      {
        $data[0]=array("");
        $data[1]=array(1);
        $empty=true;
      }
    else
      $empty=false; //mark plot as empty for color proper color selection

    // Create Plot
    $p1 = new PiePlot3D($data[1]);
    $p1->SetLabels($data[0]);
    $p1->SetLabelPos(1);
    $p1->SetLabelType(PIE_VALUE_PER);
    $p1->value->SetFont(FF_ARIAL,FS_NORMAL,12);

    foreach ($data[0] as $d)
      $colors[]=$this->severityToColor($d);

    if ($empty)
      $colors=array('#aaaaaa');

    $p1->SetSliceColors($colors);
    $p1->SetLabelType(1);
    $p1->SetSize(0.50);
    $p1->value->SetFormat("%d");

    // Create the Pie Graph.
    $graph = new PieGraph($this->params->width,$this->params->height,'auto');

    // Set different title for an empty plot
    if ($empty)
      $graph->title->Set("No data matching your criteria");
    else
      $graph->title->Set($this->params->title);

    $graph->title->SetMargin(8);
    $graph->title->SetFont(FF_VERDANA,FS_BOLD,12);
    $graph->title->SetColor("darkred");
    $graph->SetAntiAliasing();
    $graph->Add($p1);
    return $graph;
  }

  private function createAlertTypeChart()
  {
    // Setup the graph.
    $graph = new Graph($this->params->width,$this->params->height);
    $graph->img->SetMargin(60,55,95,95);
    $graph->SetScale("textlog");
    $graph->SetAngle(90);
    $graph->SetMarginColor("white");

    // Set up the title for the graph
    $graph->title->Set("Alert count by alert type (log scale)");
    $graph->title->SetMargin(8);
    $graph->title->SetFont(FF_VERDANA,FS_BOLD,12);
    $graph->title->SetColor("darkred");

    // Setup font for axis
    $graph->xaxis->SetFont(FF_VERDANA,FS_NORMAL,10);
    $graph->yaxis->SetFont(FF_VERDANA,FS_NORMAL,10);

    // Show 0 label on Y-axis (default is not to show)
    $graph->yscale->ticks->SupressZeroLabel(false);

    $data=$this->issueQuery2d($this->params,null);

    // Setup X-axis labels
    $graph->xaxis->SetTickLabels($data[0]);
    $graph->xaxis->SetLabelAngle(0);
    $graph->xaxis->SetTitleMargin(0);
    $graph->xaxis->SetLabelMargin(-50);
    $graph->xaxis->SetLabelAlign('left','center');

    // Create the bar pot
    $bplot = new BarPlot($data[1]);
    $bplot->SetWidth(0.6);
    $bplot->value->Show();
    $bplot->value->SetFont(FF_ARIAL,FS_BOLD,8);
    $bplot->value->SetFormat("%d");
    $bplot->value->SetColor("black");
    $bplot->SetValuePos('bottom');

    // Setup color for gradient fill style
    $bplot->SetFillGradient("navy:1.9","navy:1.6",GRAD_HOR);

    // Set color for the frame of each bar
    $bplot->SetColor("white");
    $graph->Add($bplot);

    return $graph;
  }

  private function createAlertTimeSeries()
  {
    $graph = new Graph($this->params->width,$this->params->height);
    $graph->SetMargin(50,40,30,130);
    $graph->SetScale('datlin',0,100);
    $graph->title->Set($params->title);

    $severities=explode(".",$this->params->qparam->severities);

    $params=$this->params;

    foreach ($severities as $s)
      {
        $params->qparam->severities=$s;
        $data[0][]=$this->issueQuery2dTime($params);
        $data[1][]=$s;
      }

    $count=count($data[0]);

    $maxval=0;
    for ($i=0; $i<$count; $i++)
      $maxval+=max($data[0][$i][1]);

    if ($maxval==0)
      {
        $led = new DigitalLED74(2);
        $led->SetSupersampling(5);
        $led->StrokeNumber('NO DATA. Maybe database is empty?',LEDC_GREEN);
        die();
      }

    $graph->SetScale('datlin',0,$maxval);
    $graph->xaxis->SetLabelAngle(90);

    for ($i=0; $i<$count; $i++)
      {
        if (count($data[0][$i][1])==0)
          continue;
        $line[] = new LinePlot($data[0][$i][1],$data[0][$i][0]);
        end($line)->setLegend($data[1][$i]);
        end($line)->SetFillColor($this->severityToColor($data[1][$i]));
      }

    $accplot = new AccLinePlot($line);

    $graph->Add($accplot);
    return $graph;
  }

  private $params;
}

?>
