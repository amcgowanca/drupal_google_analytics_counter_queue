<?php
/**
 * @file
 * GoogleAnalyticsCounterQueueGoogleAnalyticsAuthentication.inc
 */

/**
 * Class GoogleAnalyticsCounterQueueGoogleAnalyticsAuthentication
 */
class GoogleAnalyticsCounterQueueGoogleAnalyticsAuthentication extends GoogleAnalyticsCounterQueueGoogleApiQuery {
  /**
   *
   * @access public
   *
   * @param GoogleAnalyticsCounterQueueGoogleApiClient $client
   *   The API client to retrieve the request token for.
   *
   * @return GoogleAnalyticsCounterQueueGoogleApiResults
   *   Returns the API results object instance for this request.
   */
  public static function getRequestToken(GoogleAnalyticsCounterQueueGoogleApiClient $client) {
    $request = new self($client, 'accounts/OAuthGetRequestToken');
    $request->setParameters(array(
        'scope' => 'https://www.googleapis.com/auth/analytics.readonly',
        'xoauth_displayname' => google_analytics_counter_queue_xoauth_display_name(),
        'oauth_callback' => google_analytics_counter_queue_oauth_callback_url(),
    ))
      ->execute();
    return $request->getQueryResult();
  }

  /**
   * Redirects the end user to Google accounts to allow authorization.
   *
   * @access public
   *
   * @param GoogleAnalyticsCounterQueueGoogleApiClient $client
   *   The API client to obtain authorization for.
   */
  public static function obtainAuthorization(GoogleAnalyticsCounterQueueGoogleApiClient $client) {
    drupal_goto('https://www.google.com/accounts/OAuthAuthorizeToken', array(
      'query' => array(
        'oauth_token' => $client->getConsumer()->key,
        'hd' => 'default',
      ))
    );
  }

  /**
   *
   * @access public
   *
   * @param GoogleAnalyticsCounterQueueGoogleApiClient $client
   *   The API client to retrieve the access token for.
   *
   * @return GoogleAnalyticsCounterQueueGoogleApiResults
   *   Returns the API results object instance for this request.
   */
  public static function getAccessToken(GoogleAnalyticsCounterQueueGoogleApiClient $client) {
    $request = new self($client, 'accounts/OAuthGetAccessToken');
    $request->setParameter('oauth_verifier', $client->getVerifier())
      ->setMethod('GET')
      ->execute();
    return $request->getQueryResult();
  }

  /**
   * Performs an API query to revoke the access token of the specified client.
   *
   * @access public
   *
   * @param GoogleAnalyticsCounterQueueGoogleApiClient $client
   *   The API client to revoke its access token.
   */
  public static function revokeToken(GoogleAnalyticsCounterQueueGoogleApiClient $client) {
    // TODO: Determine if more is needed here... always receives a 400?
    $request = new self($client, 'accounts/AuthSubRevokeToken');
    $request->setMethod('GET')
      ->execute();
  }

  /**
   * {@inheritdoc}
   */
  public function getEndpointHost() {
    return 'https://www.google.com';
  }
}
