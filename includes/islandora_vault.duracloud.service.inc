<?php

/**
 * tries to deploy a duracloud service
 * @param string $id
 * NOT USING THS FUNCTION
 * @return string 
 */
function islandora_vault_deploy_service($id) {
  $dcs = DuracloudService::getInstance();
  $space = $dcs->getSpace();
  $service_config = <<<EOD
<ns1:service xmlns:ns1="duracloud.org" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
  schemaVersion="0.8.0" id="0" serviceVersion="0.8.0" contentId="fixityservice-0.8.0.zip"
  displayName="FixityService" userConfigVersion="1.0">
  <userConfig id="0">
    <modeSet name="mode" displayName="">
      <mode name="all-in-one-for-space" displayName="" selected="true">
        <property name="storeId" input="TEXT" value="0"/>
        <property name="targetSpaceId" input="TEXT" value="$space"/>
        <property name="providedListingSpaceIdA" input="TEXT" value=""/>
        <property name="providedListingContentIdA" input="TEXT" value=""/>
      </mode>
    </modeSet>
  </userConfig>
</ns1:service>
EOD;
  try {
    $rt = $dcs->deployService('0', $service_config);
  } catch (Exception $e) {
    drupal_set_message(t('Error deploying Service %s', array('%s' => $e->getMessage())));
    return "";
  }
  return $rt;
}


/**
 * gets a report name from the summary reports list
 * gets report for the space we are managing
 * @return type 
 */
function islandora_vault_get_latest_report_id($duraboss) {
  $report_list = $duraboss->getCompletedServicesReport();
  $xml = new DomDocument();
  $xml->loadXML($report_list);
  $xpath = new DOMXPath($xml);
  $space = $duraboss->getSpace();
  $xpath->registerNamespace('dur', 'duracloud.org');
  $nodes = $xpath->query("//configs/property[@name='Space containing content items']");
  $length = $nodes->length;
  $report_name = NULL;
  foreach ($nodes as $node) {
    $tst = $node->nodeValue;
    if ($tst == $space) {
      $config = $node->parentNode;
      $summary = $config->parentNode;
      $sxml = simplexml_import_dom($summary);
      foreach ($sxml->properties->property as $p) {
        if ($p['name'] == 'Report') {
          $report_name = (string) $p;
          return $report_name;//assuming first report we find is the right one
          //this assumption may need to be revisted
        }
      }
    }
  }
  return $report_name;
}
