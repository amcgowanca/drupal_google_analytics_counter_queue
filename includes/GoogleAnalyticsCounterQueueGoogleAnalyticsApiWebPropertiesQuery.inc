<?php
/**
 * @file
 * GoogleAnalyticsCounterQueueGoogleAnalyticsApiWebPropertiesQuery.inc
 */

/**
 * Class GoogleAnalyticsCounterQueueGoogleAnalyticsApiWebPropertiesQuery
 */
class GoogleAnalyticsCounterQueueGoogleAnalyticsApiWebPropertiesQuery extends GoogleAnalyticsCounterQueueGoogleAnalyticsApiQuery {
  /**
   * {@inheritdoc}
   */
  public function __construct(GoogleAnalyticsCounterQueueGoogleApiClient $client) {
    parent::__construct($client, NULL);
  }

  /**
   * {@inheritdoc}
   */
  public function getEndpointPath() {
    $account_id = $this->getParameter('account-id', '~all');
    return 'management/accounts/' . $account_id . '/webproperties';
  }

  /**
   * {@inheritdoc}
   */
  public function getDefaultParameters() {
    return array(
      'account-id' => '~all',
    );
  }
}
