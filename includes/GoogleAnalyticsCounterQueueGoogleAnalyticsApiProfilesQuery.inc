<?php
/**
 * @file
 * GoogleAnalyticsCounterQueueGoogleAnalyticsApiProfilesQuery.inc
 */

/**
 * Class GoogleAnalyticsCounterQueueGoogleAnalyticsApiProfilesQuery
 */
class GoogleAnalyticsCounterQueueGoogleAnalyticsApiProfilesQuery extends GoogleAnalyticsCounterQueueGoogleAnalyticsApiQuery {
  /**
   * {@inheritdoc}
   */
  public function getEndpointPath() {
    $account_id = $this->getParameter('account-id', '~all');
    $webprop_id = $this->getParameter('web-property-id', '~all');
    return 'management/accounts/' . $account_id . '/webproperties/' . $webprop_id . '/profiles';
  }

  /**
   * {@inheritdoc}
   */
  public function getDefaultParameters() {
    return array(
      'account-id' => '~all',
      'web-property-id' => '~all',
    );
  }
}
