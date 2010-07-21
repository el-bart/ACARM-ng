<?xml version="1.0" encoding="utf-8"?>
<?xml-stylesheet type="text/css" href="style.css"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.1//EN" "http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="pl">
<head>
  <title>ACARM-ng</title>
  <link href="style.css" rel="stylesheet" type="text/css"/>
  <meta http-equiv="Content-Type" content="text/html; charset=UTF-8"/>
</head>

<body>

<div id="mainDiv">

  <com:TForm>
  <div id="templateDiv">
    <com:MainBarTemplate />
  </div>

  <div id="contentDiv">
    <table id="contentTable">
      <tr id="contentTR" valign="top">
        <!-- LEFT MENU -->
        <td id="sideMenuPart">
          <!-- MENU HEADER -->
          <div id="lefthead">
          MENU
          </div>
          <!-- MENU ITSELF -->
          <com:MainMenuTemplate />
        </td>
        <!-- CONTENT -->
        <td rowspan="2" id="mainContentPart">
          <com:TContentPlaceHolder ID="content" />
        </td>
      </tr>
    </table>
  </div>

  <div id="footerDiv">
    <table id="footerBar">
      <tr>
        <td id="w3cValidator">
          <a href="http://validator.w3.org/check?uri=referer"><img
             src="http://www.w3.org/Icons/valid-xhtml11-blue"
             class="FooterLogoImage"
             alt="Valid XHTML 1.1"/></a>
          <a href="http://jigsaw.w3.org/css-validator/"><img
             class="FooterLogoImage"
             src="http://jigsaw.w3.org/css-validator/images/vcss-blue"
             alt="Valid CSS!"/></a>
        </td>
        <td id="logos">
          <a href="http://www.acarm.wcss.wroc.pl" target="new_window_acarmng"><img src="pics/acarm-ng_logo.png" class="FooterLogoImage" alt="ACARM-ng project's page"/></a>
          <a href="http://www.wcss.wroc.pl"       target="new_window_wcss">   <img src="pics/logo_small.png"    class="FooterLogoImage" alt="Wrocław Centre for Networking and Supercomputing"/></a>
        </td>
      </tr>
    </table>
  <!-- end of FooterDiv -->
  </div>
  </com:TForm>

</div>

</body>
</html>