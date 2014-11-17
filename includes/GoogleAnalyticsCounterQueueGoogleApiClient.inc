<?php
/**
 * @file
 * GoogleAnalyticsCounterQueueGoogleAnalyticsOAuthClient.inc
 */

/**
 *
 */
class GoogleAnalyticsCounterQueueGoogleApiClient {
  /**
   * @var OAuthSignatureMethod_HMAC_SHA1
   */
  private $signature_method = NULL;

  /**
   * @var GoogleAnalyticsCounterQueueOAuthToken
   */
  private $consumer;

  /**
   * @var GoogleAnalyticsCounterQueueOAuthToken
   */
  private $token;

  /**
   * @var string
   */
  private $verifier;

  /**
   * Creates a new instance of GoogleAnalyticsCounterQueueAnalyticsOAuthClient.
   *
   * @access public
   *
   * @param GoogleAnalyticsCounterQueueOAuthToken $consumer
   *   The oauth consumer token object.
   * @param GoogleAnalyticsCounterQueueOAuthToken $token
   *   The oauth token object.
   */
  public function __construct(GoogleAnalyticsCounterQueueOAuthToken $consumer, GoogleAnalyticsCounterQueueOAuthToken $token = NULL) {
    $this->signature_method = new OAuthSignatureMethod_HMAC_SHA1();
    $this->consumer = $consumer;

    if (!empty($token)) {
      $this->token = $token;
    }
  }

  public function getConsumer() {
    return $this->consumer;
  }

  public function getSignatureMethod() {
    return $this->signature_method;
  }

  public function getVerifier() {
    return $this->verifier;
  }

  public function setVerifier($verifier) {
    $this->verifier = $verifier;
    return $this;
  }

  public function getToken() {
    return $this->token;
  }

  /**
   * Checks if this instance is currently authenticated.
   *
   * @access public
   *
   * @return bool
   *   Returns TRUE if authenticated, otherwise FALSE.
   */
  public function isAuthenticated() {
    // TODO: Determine if there is a better way for handling this determination.
    if (!empty($this->token)) {
      if ('anonymous' != $this->token->key && 'anonymous' != $this->token->secret) {
        return TRUE;
      }
    }
    return FALSE;
  }

  /**
   * @return GoogleAnalyticsCounterQueueOAuthToken
   * @throws Exception
   */
  public function getRequestToken() {
    if (empty($this->token)) {
      try {
        $result = google_analytics_counter_queue_api_auth_request_token($this);
        if ('200' == $result['code'] && !empty($result['data'])) {
          $data = array();
          parse_str($result['data'], $data);
          $this->consumer = new GoogleAnalyticsCounterQueueOAuthToken($data['oauth_token'], $data['oauth_token_secret']);
        }
      }
      catch (Exception $exception) {
        throw $exception;
      }
    }

    return $this->getConsumer();
  }

  /**
   *
   */
  public function obtainAuthorization() {
    google_analytics_counter_queue_api_auth_obtain_auth($this);
  }

  /**
   * @access public
   * @throws Exception
   */
  public function getAccessToken() {
    // TODO: Add condition for access_token property so this is not happening all the time.
    try {
      $result = google_analytics_counter_queue_api_auth_access_token($this);
      if (!empty($result['data'])) {
        $data = array();
        parse_str($result['data'], $data);
        $this->token = new GoogleAnalyticsCounterQueueOAuthToken($data['oauth_token'], $data['oauth_token_secret']);

        variable_set('google_analytics_counter_queue_api_oauth_token', $data['oauth_token']);
        variable_set('google_analytics_counter_queue_api_oauth_token_secret', $data['oauth_token_secret']);
      }
      else {
        // TODO: Cleanup.
        throw new Exception(t('Unable to retrieve access token?'));
      }
    }
    catch (Exception $exception) {
      throw $exception;
    }

    return $this->token;
  }

  /**
   * @access public
   */
  public function revokeToken() {
    google_analytics_counter_queue_api_auth_revoke($this);
    // Reset the client's properties that need to be.
    $this->token = NULL;
    $this->verifier = NULL;
    // Notify other modules that authentication has been revoked.
    google_analytics_counter_queue_module_include();
    module_invoke_all('google_analytics_counter_queue_api_revoked');
  }
}
