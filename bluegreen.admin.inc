<?php
/**
 * @file
 * Admin UI for Blue-Green Deployment module.
 */

/**
 * Form builder for the blue-green deployment admin page.
 */
function bluegreen_admin_form($form, &$form_state) {
  // Check if blue-green is configured
  if (!bluegreen_is_configured()) {
    backdrop_set_message(t('Blue-Green deployment has not been set up yet.'), 'warning');
    $setup_url = url('admin/config/development/bluegreen/setup');
    $form['setup_message'] = array(
      '#markup' => '<h2>' . t('Welcome to Blue-Green Deployment') . '</h2>' .
        '<p>' . t('Blue-Green deployment is not configured yet. Use the Setup tab to initialize blue-green deployment for this site.') . '</p>' .
        '<p><a href="' . $setup_url . '" class="button button-primary">' . t('Go to Setup') . '</a></p>',
    );
    return $form;
  }

  $active = bluegreen_get_active_environment();
  $idle = bluegreen_get_idle_environment();
  
  $blue_info = bluegreen_get_environment_info('blue');
  $green_info = bluegreen_get_environment_info('green');
  
  $blue_installed = bluegreen_environment_is_installed('blue');
  $green_installed = bluegreen_environment_is_installed('green');
  
  $blue_last_sync = bluegreen_get_last_sync('blue');
  $green_last_sync = bluegreen_get_last_sync('green');

  $form['#tree'] = TRUE;

  $form['environments'] = array(
    '#type' => 'container',
    '#attributes' => array('class' => array('bluegreen-environments-wrapper')),
  );

  // Blue Environment Panel
  $form['environments']['blue'] = array(
    '#type' => 'container',
    '#attributes' => array('class' => array('bluegreen-environment', 'bluegreen-blue')),
  );

  $form['environments']['blue']['header'] = array(
    '#markup' => '<h2>' . t('Blue Environment') . '</h2>',
  );

  $form['environments']['blue']['status'] = array(
    '#markup' => '<div class="environment-status ' . ($active === 'blue' ? 'status-live' : 'status-idle') . '">' .
      '<span class="status-indicator"></span> ' .
      ($active === 'blue' ? t('Live') : t('Idle (Dev)')) .
      '</div>',
  );

  // Get displayable prefix value (handle both string and array)
  $blue_prefix_display = '';
  if (is_array($blue_info['prefix'])) {
    $blue_prefix_display = isset($blue_info['prefix']['default']) ? $blue_info['prefix']['default'] : '';
  }
  else {
    $blue_prefix_display = !empty($blue_info['prefix']) ? $blue_info['prefix'] : '';
  }
  
  $form['environments']['blue']['info'] = array(
    '#markup' => '<div class="environment-info">' .
      '<div class="info-line"><strong>' . t('Database:') . '</strong> ' . check_plain($blue_info['database']) . '</div>' .
      '<div class="info-line"><strong>' . t('Prefix:') . '</strong> ' . ($blue_prefix_display ? check_plain($blue_prefix_display) : t('(none)')) . '</div>' .
      '<div class="info-line"><strong>' . t('Status:') . '</strong> ' . ($blue_installed ? t('Installed') : t('Not installed')) . '</div>' .
      ($blue_last_sync ? '<div class="info-line"><strong>' . t('Last synced:') . '</strong> ' . format_date($blue_last_sync, 'short') . '</div>' : '') .
      '</div>',
  );

  $form['environments']['blue']['actions'] = array(
    '#type' => 'container',
    '#attributes' => array('class' => array('environment-actions')),
  );

  // Only show backup button if backup module is enabled
  if (module_exists('backup')) {
    $form['environments']['blue']['actions']['backup'] = array(
      '#type' => 'submit',
      '#value' => t('Backup'),
      '#submit' => array('bluegreen_backup_submit'),
      '#environment' => 'blue',
      '#attributes' => array('class' => array('button-secondary')),
    );
  }

  // Show sync button if Blue is idle (not active)
  if ($active !== 'blue') {
    $form['environments']['blue']['actions']['sync'] = array(
      '#type' => 'submit',
      '#value' => t('Sync from @env', array('@env' => ucfirst($active))),
      '#submit' => array('bluegreen_sync_submit'),
      '#from' => $active,
      '#to' => 'blue',
      '#attributes' => array('class' => array('button-secondary')),
    );
  }

  // Green Environment Panel
  $form['environments']['green'] = array(
    '#type' => 'container',
    '#attributes' => array('class' => array('bluegreen-environment', 'bluegreen-green')),
  );

  $form['environments']['green']['header'] = array(
    '#markup' => '<h2>' . t('Green Environment') . '</h2>',
  );

  $form['environments']['green']['status'] = array(
    '#markup' => '<div class="environment-status ' . ($active === 'green' ? 'status-live' : 'status-idle') . '">' .
      '<span class="status-indicator"></span> ' .
      ($active === 'green' ? t('Live') : t('Idle (Dev)')) .
      '</div>',
  );

  // Get displayable prefix value (handle both string and array)
  $green_prefix_display = '';
  if (is_array($green_info['prefix'])) {
    $green_prefix_display = isset($green_info['prefix']['default']) ? $green_info['prefix']['default'] : '';
  }
  else {
    $green_prefix_display = !empty($green_info['prefix']) ? $green_info['prefix'] : '';
  }
  
  $form['environments']['green']['info'] = array(
    '#markup' => '<div class="environment-info">' .
      '<div class="info-line"><strong>' . t('Database:') . '</strong> ' . check_plain($green_info['database']) . '</div>' .
      '<div class="info-line"><strong>' . t('Prefix:') . '</strong> ' . ($green_prefix_display ? check_plain($green_prefix_display) : t('(none)')) . '</div>' .
      '<div class="info-line"><strong>' . t('Status:') . '</strong> ' . ($green_installed ? t('Installed') : t('Not installed')) . '</div>' .
      ($green_last_sync ? '<div class="info-line"><strong>' . t('Last synced:') . '</strong> ' . format_date($green_last_sync, 'short') . '</div>' : '') .
      '</div>',
  );

  $form['environments']['green']['actions'] = array(
    '#type' => 'container',
    '#attributes' => array('class' => array('environment-actions')),
  );

  // Only show backup button if backup module is enabled
  if (module_exists('backup')) {
    $form['environments']['green']['actions']['backup'] = array(
      '#type' => 'submit',
      '#value' => t('Backup'),
      '#submit' => array('bluegreen_backup_submit'),
      '#environment' => 'green',
      '#attributes' => array('class' => array('button-secondary')),
    );
  }

  // Show sync button if Green is idle (not active)
  if ($active !== 'green') {
    $form['environments']['green']['actions']['sync'] = array(
      '#type' => 'submit',
      '#value' => t('Sync from @env', array('@env' => ucfirst($active))),
      '#submit' => array('bluegreen_sync_submit'),
      '#from' => $active,
      '#to' => 'green',
      '#attributes' => array('class' => array('button-secondary')),
    );
  }

  // Switch Environment Section
  $form['switch_section'] = array(
    '#type' => 'container',
    '#attributes' => array('class' => array('bluegreen-switch-section')),
  );

  // Only show switch button if idle environment is installed
  if (bluegreen_environment_is_installed($idle)) {
    $form['switch_section']['switch_button'] = array(
      '#type' => 'submit',
      '#value' => t('Make @env Environment Live', array('@env' => ucfirst($idle))),
      '#submit' => array('bluegreen_switch_submit'),
      '#attributes' => array('class' => array('button-primary', 'bluegreen-switch-button')),
    );

    $form['switch_section']['help'] = array(
      '#markup' => '<p class="switch-help">' . 
        t('This will switch your live site to the @env environment. You can switch back anytime.', 
          array('@env' => $idle)) . 
        '</p>',
    );
  }
  else {
    $form['switch_section']['message'] = array(
      '#markup' => '<div class="messages warning">' . 
        t('The @env environment must be synced before you can switch to it.', 
          array('@env' => $idle)) . 
        '</div>',
    );
  }

  $form['warning'] = array(
    '#markup' => '<p class="bluegreen-warning">' . 
      t('Ensure no one else is editing content during a swap.') . 
      '</p>',
  );

  return $form;
}

/**
 * Submit handler for backup action.
 */
function bluegreen_backup_submit($form, &$form_state) {
  $environment = $form_state['triggering_element']['#environment'];
  
  // Use Backdrop's backup functionality
  module_load_include('inc', 'backup', 'backup.backup');
  
  try {
    $backup_file = backup_create_backup();
    if ($backup_file) {
      backdrop_set_message(t('Backup created successfully for @env environment.', 
        array('@env' => ucfirst($environment))));
    }
    else {
      backdrop_set_message(t('Backup failed for @env environment.', 
        array('@env' => ucfirst($environment))), 'error');
    }
  }
  catch (Exception $e) {
    backdrop_set_message(t('Error creating backup: @message', 
      array('@message' => $e->getMessage())), 'error');
  }
}

/**
 * Submit handler for sync action.
 */
function bluegreen_sync_submit($form, &$form_state) {
  $from = $form_state['triggering_element']['#from'];
  $to = $form_state['triggering_element']['#to'];
  
  // Set up batch operation
  $batch = array(
    'title' => t('Syncing @to environment from @from', 
      array('@to' => $to, '@from' => $from)),
    'operations' => array(
      array('bluegreen_sync_batch_operation', array($from, $to)),
    ),
    'finished' => 'bluegreen_sync_batch_finished',
    'file' => backdrop_get_path('module', 'bluegreen') . '/bluegreen.admin.inc',
  );
  
  batch_set($batch);
}

/**
 * Batch operation callback for syncing environments.
 */
function bluegreen_sync_batch_operation($from, $to, &$context) {
  if (!isset($context['sandbox']['progress'])) {
    $context['sandbox']['progress'] = 0;
    $context['sandbox']['max'] = 1;
    $context['results']['from'] = $from;
    $context['results']['to'] = $to;
  }

  try {
    bluegreen_sync_database($from, $to);
    bluegreen_set_last_sync($to);
    $context['results']['success'] = TRUE;
  }
  catch (Exception $e) {
    $context['results']['success'] = FALSE;
    $context['results']['error'] = $e->getMessage();
  }

  $context['sandbox']['progress']++;
  $context['finished'] = 1;
}

/**
 * Batch finished callback for syncing environments.
 */
function bluegreen_sync_batch_finished($success, $results, $operations) {
  if (!empty($results['success'])) {
    backdrop_set_message(t('Successfully synced @to environment from @from.', 
      array('@to' => $results['to'], '@from' => $results['from'])));
  }
  else {
    backdrop_set_message(t('Error syncing environments: @error', 
      array('@error' => $results['error'])), 'error');
  }
}

/**
 * Submit handler for environment switch.
 */
function bluegreen_switch_submit($form, &$form_state) {
  $current = bluegreen_get_active_environment();
  $new = bluegreen_get_idle_environment();
  
  try {
    bluegreen_switch_environment($new);
    backdrop_set_message(t('Successfully switched to @env environment!', 
      array('@env' => ucfirst($new))), 'status');
    backdrop_set_message(t('The @env environment is now live. All visitors will see content from the @env database tables.', 
      array('@env' => $new)), 'status');
    
    // Redirect to cache clear page first, then to admin page
    $form_state['redirect'] = 'admin/config/development/bluegreen/clear-cache';
  }
  catch (Exception $e) {
    backdrop_set_message(t('Error switching environments: @message', 
      array('@message' => $e->getMessage())), 'error');
  }
}

/**
 * Sync database from one environment to another.
 *
 * @param string $from
 *   Source environment ('blue' or 'green').
 * @param string $to
 *   Target environment ('blue' or 'green').
 *
 * @throws Exception
 */
function bluegreen_sync_database($from, $to) {
  $from_info = bluegreen_get_environment_info($from);
  $to_info = bluegreen_get_environment_info($to);
  
  if (!$from_info || !$to_info) {
    throw new Exception(t('Invalid environment configuration.'));
  }

  $from_prefix = !empty($from_info['prefix']) ? $from_info['prefix'] : '';
  $to_prefix = !empty($to_info['prefix']) ? $to_info['prefix'] : '';
  
  // Get list of source tables
  $source_tables = db_find_tables($from_prefix . '%');
  
  if (empty($source_tables)) {
    throw new Exception(t('No tables found in source environment.'));
  }

  // Get list of shared tables to exclude from sync
  $shared_tables = bluegreen_get_shared_tables();

  // Drop all existing tables in target environment (except shared tables)
  $target_tables = db_find_tables($to_prefix . '%');
  foreach ($target_tables as $table) {
    $base_name = substr($table, strlen($to_prefix));
    if (!in_array($base_name, $shared_tables)) {
      db_query("DROP TABLE IF EXISTS {$table}");
    }
  }

  // Copy each table from source to target (except shared tables)
  foreach ($source_tables as $source_table) {
    // Determine target table name by replacing prefix
    $base_name = substr($source_table, strlen($from_prefix));
    
    // Skip shared tables
    if (in_array($base_name, $shared_tables)) {
      continue;
    }
    
    $target_table = $to_prefix . $base_name;
    
    // Create table structure
    db_query("CREATE TABLE {$target_table} LIKE {$source_table}");
    
    // Copy data
    db_query("INSERT INTO {$target_table} SELECT * FROM {$source_table}");
  }
}

/**
 * Copy a single table between environments.
 */
function bluegreen_copy_table($from_info, $to_info, $from_table, $to_table) {
  // Ensure all required fields are set with defaults
  $from_info += array(
    'driver' => 'mysql',
    'host' => 'localhost',
    'port' => 3306,
    'username' => '',
    'password' => '',
    'prefix' => '',
  );
  $to_info += array(
    'driver' => 'mysql',
    'host' => 'localhost',
    'port' => 3306,
    'username' => '',
    'password' => '',
    'prefix' => '',
  );
  
  try {
    // For MySQL/MariaDB, use native SQL for efficiency
    Database::addConnectionInfo('target_temp', 'default', $to_info);
    Database::setActiveConnection('target_temp');
    $target = Database::getConnection('default', 'target_temp');
    
    // Drop existing table
    if ($target->schema()->tableExists($to_table)) {
      $target->schema()->dropTable($to_table);
    }
    
    // Use CREATE TABLE LIKE to copy structure, then INSERT INTO to copy data
    // Build the query with proper database.table notation
    $source_db = $from_info['database'];
    $target_db = $to_info['database'];
    
    // Create table structure
    $target->query("CREATE TABLE `{$to_table}` LIKE `{$source_db}`.`{$from_table}`");
    
    // Copy data
    $target->query("INSERT INTO `{$to_table}` SELECT * FROM `{$source_db}`.`{$from_table}`");
    
    // Clean up connection
    Database::setActiveConnection('default');
    Database::removeConnection('target_temp');
  }
  catch (Exception $e) {
    // Clean up on error
    Database::setActiveConnection('default');
    Database::removeConnection('target_temp');
    throw $e;
  }
}

/**
 * Switch the active environment.
 *
 * @param string $new_environment
 *   The environment to switch to ('blue' or 'green').
 *
 * @throws Exception
 */
function bluegreen_switch_environment($new_environment) {
  global $databases;
  
  if (!in_array($new_environment, array('blue', 'green'))) {
    throw new Exception(t('Invalid environment specified.'));
  }

  // Get the environment info to validate it exists
  $env_info = bluegreen_get_environment_info($new_environment);
  if (!$env_info) {
    throw new Exception(t('Could not get configuration for @env environment.', array('@env' => $new_environment)));
  }
  
  // Update settings.bluegreen.php to change the active prefix
  $settings_file = BACKDROP_ROOT . '/settings.bluegreen.php';
  if (!file_exists($settings_file)) {
    throw new Exception(t('Blue-Green settings file not found. Please run the setup wizard first.'));
  }
  
  $settings_content = file_get_contents($settings_file);
  $original_content = $settings_content;
  
  // Get the old and new prefix values
  // With the new shared tables system, prefix is an array
  $old_prefix_default = is_array($databases['default']['default']['prefix']) 
    ? $databases['default']['default']['prefix']['default'] 
    : $databases['default']['default']['prefix'];
  $new_prefix_default = is_array($env_info['prefix']) 
    ? $env_info['prefix']['default'] 
    : $env_info['prefix'];
  
  // Build the new prefix configuration based on which environment we're switching to
  if ($new_prefix_default === '' || $new_prefix_default === 'blue') {
    // Switching to Blue (unprefixed) - use shared table prefix only
    $new_prefix_config = '$shared_table_prefix';
  }
  else {
    // Switching to Green (alt_) - merge default with shared
    // Note: array_merge last argument wins, so put $shared_table_prefix first
    $new_prefix_config = "array_merge(\n  \$shared_table_prefix,\n  array('default' => '" . $new_prefix_default . "')\n)";
  }
  
  // Use regex to find and replace the prefix configuration line
  $pattern = '/(\\$databases\[\'default\'\]\[\'default\'\]\[\'prefix\'\]\s*=\s*)([^;]+)(;)/s';
  $replacement = '$1' . $new_prefix_config . '$3';
  
  // Do the replacement
  $settings_content = preg_replace($pattern, $replacement, $settings_content);
  
  // Check if replacement actually happened
  if ($settings_content === $original_content) {
    throw new Exception(t('Failed to update prefix in settings.bluegreen.php. Could not find the line to update.'));
  }
  
  // Write back to file
  $bytes_written = file_put_contents($settings_file, $settings_content);
  if ($bytes_written === FALSE) {
    throw new Exception(t('Could not write to settings.bluegreen.php. Please check file permissions.'));
  }
  
  // Ensure file was written
  clearstatcache(TRUE, $settings_file);
  
  watchdog('bluegreen', 'Updated settings.bluegreen.php: changed prefix from @old to @new (@bytes bytes written)', 
    array('@old' => $old_prefix_default, '@new' => $new_prefix_default, '@bytes' => $bytes_written), 
    WATCHDOG_INFO);
  
  // Use state system to store the active environment
  state_set('bluegreen_active_environment', $new_environment);

  // Log the switch
  watchdog('bluegreen', 'Switched environment to @env', 
    array('@env' => $new_environment), WATCHDOG_INFO);
  
  // Note: We don't clear caches here because the database prefix just changed
  // and cache clearing would fail. The redirect and page load will handle cache rebuilding.
}

/**
 * Menu callback for switch environment.
 */
function bluegreen_switch_environment_callback($environment) {
  try {
    bluegreen_switch_environment($environment);
    backdrop_set_message(t('Successfully switched to @env environment.', 
      array('@env' => ucfirst($environment))));
  }
  catch (Exception $e) {
    backdrop_set_message(t('Error: @message', array('@message' => $e->getMessage())), 'error');
  }
  
  backdrop_goto('admin/config/development/bluegreen');
}

/**
 * Menu callback for sync environments.
 */
function bluegreen_sync_environment_callback($from, $to) {
  try {
    bluegreen_sync_database($from, $to);
    bluegreen_set_last_sync($to);
    backdrop_set_message(t('Successfully synced @to from @from.', 
      array('@to' => $to, '@from' => $from)));
  }
  catch (Exception $e) {
    backdrop_set_message(t('Error: @message', array('@message' => $e->getMessage())), 'error');
  }
  
  backdrop_goto('admin/config/development/bluegreen');
}

/**
 * Menu callback for backup environment.
 */
function bluegreen_backup_environment_callback($environment) {
  module_load_include('inc', 'backup', 'backup.backup');
  
  try {
    $backup_file = backup_create_backup();
    if ($backup_file) {
      backdrop_set_message(t('Backup created successfully for @env environment.', 
        array('@env' => ucfirst($environment))));
    }
    else {
      backdrop_set_message(t('Backup failed.'), 'error');
    }
  }
  catch (Exception $e) {
    backdrop_set_message(t('Error: @message', array('@message' => $e->getMessage())), 'error');
  }
  
  backdrop_goto('admin/config/development/bluegreen');
}

/**
 * Menu callback to clear cache after environment switch.
 */
function bluegreen_clear_cache_callback() {
  // Clear all caches now that the new database prefix is active
  backdrop_flush_all_caches();
  
  backdrop_set_message(t('Cache cleared for new environment.'), 'status');
  backdrop_goto('admin/config/development/bluegreen');
}

/**
 * Form builder for blue-green deployment setup.
 */
function bluegreen_setup_form($form, &$form_state) {
  global $databases;
  
  // Check if already configured
  if (bluegreen_is_configured()) {
    $form['message'] = array(
      '#markup' => '<p>' . t('This site is configured for blue-green deployment. Use the Overview tab to monitor or switch environments.') . '</p>',
    );
    return $form;
  }
  
  // Get current database configuration
  $current_db = $databases['default']['default'];
  $current_prefix = !empty($current_db['prefix']) ? $current_db['prefix'] : '';
  
  $form['intro'] = array(
    '#markup' => '<h2>' . t('Set Up Blue-Green Deployment') . '</h2>' .
      '<p>' . t('This wizard will configure your site for blue-green deployment by:') . '</p>' .
      '<ol>' .
      '<li>' . t('Duplicating your current database tables with alt_ prefix (for green environment)') . '</li>' .
      '<li>' . t('Creating settings.bluegreen.php with environment configurations') . '</li>' .
      '<li>' . t('Setting up the state tracking system') . '</li>' .
      '</ol>' .
      '<p>' . t('<strong>Note:</strong> Blue environment = unprefixed tables (current), Green environment = alt_ prefixed tables (alternate)') . '</p>',
  );
  
  $form['current_info'] = array(
    '#type' => 'fieldset',
    '#title' => t('Current Database Configuration'),
  );
  
  $form['current_info']['info'] = array(
    '#markup' => '<dl>' .
      '<dt><strong>' . t('Database:') . '</strong></dt><dd>' . check_plain($current_db['database']) . '</dd>' .
      '<dt><strong>' . t('Host:') . '</strong></dt><dd>' . check_plain($current_db['host']) . '</dd>' .
      '<dt><strong>' . t('Current Prefix:') . '</strong></dt><dd>' . ($current_prefix ? check_plain($current_prefix) : t('(none)')) . '</dd>' .
      '</dl>',
  );
  
  // Count current tables
  $table_count = count(db_find_tables($current_prefix . '%'));
  $form['current_info']['table_count'] = array(
    '#markup' => '<p><strong>' . t('Current table count:') . '</strong> ' . $table_count . '</p>',
  );
  
  $form['initial_active'] = array(
    '#type' => 'radios',
    '#title' => t('Which environment should be initially active?'),
    '#options' => array(
      'blue' => t('<strong>Blue</strong> - Current tables will be the blue environment'),
      'green' => t('<strong>Green</strong> - Current tables will be the green environment'),
    ),
    '#default_value' => 'blue',
    '#required' => TRUE,
    '#description' => t('The active environment will serve your live site. The other environment will be created as a copy for testing.'),
  );
  
  $form['warning'] = array(
    '#markup' => '<div class="messages warning">' .
      t('<strong>Important:</strong> This process will:') .
      '<ul>' .
      '<li>' . t('Create duplicate tables with alt_ prefix (may take several minutes for large databases)') . '</li>' .
      '<li>' . t('Create settings.bluegreen.php and modify settings.php to include it') . '</li>' .
      '<li>' . t('This action cannot be easily undone') . '</li>' .
      '</ul>' .
      '</div>',
  );
  
  $form['confirm'] = array(
    '#type' => 'checkbox',
    '#title' => t('I understand and want to proceed with setup'),
    '#required' => TRUE,
  );
  
  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Set Up Blue-Green Deployment'),
  );
  
  return $form;
}

/**
 * Form submit handler for bluegreen_setup_form().
 */
function bluegreen_setup_form_submit($form, &$form_state) {
  $initial_active = $form_state['values']['initial_active'];
  
  // Use batch API for table duplication
  $batch = array(
    'title' => t('Setting up Blue-Green Deployment'),
    'operations' => array(
      array('bluegreen_setup_duplicate_tables', array($initial_active)),
      array('bluegreen_setup_write_settings', array($initial_active)),
      array('bluegreen_setup_finalize', array($initial_active)),
    ),
    'finished' => 'bluegreen_setup_finished',
    'file' => backdrop_get_path('module', 'bluegreen') . '/bluegreen.admin.inc',
  );
  
  batch_set($batch);
}

/**
 * Batch operation: Duplicate tables for both environments.
 */
function bluegreen_setup_duplicate_tables($initial_active, &$context) {
  global $databases;
  
  if (!isset($context['sandbox']['progress'])) {
    // Initialize batch
    $current_db = $databases['default']['default'];
    $current_prefix = !empty($current_db['prefix']) ? $current_db['prefix'] : '';
    
    // Get list of all current tables
    $context['sandbox']['tables'] = db_find_tables($current_prefix . '%');
    $context['sandbox']['progress'] = 0;
    $context['sandbox']['current_prefix'] = $current_prefix;
    $context['sandbox']['max'] = count($context['sandbox']['tables']);
    $context['sandbox']['initial_active'] = $initial_active;
    
    $context['results']['tables_created'] = 0;
  }
  
  // Process tables in chunks
  $tables = $context['sandbox']['tables'];
  $current_prefix = $context['sandbox']['current_prefix'];
  $initial_active = $context['sandbox']['initial_active'];
  
  // Process up to 10 tables per batch
  $limit = min(10, $context['sandbox']['max'] - $context['sandbox']['progress']);
  $tables_to_process = array_slice($tables, $context['sandbox']['progress'], $limit);
  
  // Get list of shared tables that should NOT be duplicated
  $shared_tables = bluegreen_get_shared_tables();
  
  foreach ($tables_to_process as $table) {
    try {
      // Determine the base table name (without current prefix)
      $base_name = $current_prefix ? substr($table, strlen($current_prefix)) : $table;
      
      // Skip shared tables - they should exist only once
      if (in_array($base_name, $shared_tables)) {
        $context['sandbox']['progress']++;
        continue;
      }
      
      // Create alt_ prefixed tables for the green/alternate environment
      // Blue environment will use the original unprefixed tables
      $alt_table = 'alt_' . $base_name;
      
      // Copy to alternate (green) environment
      db_query("CREATE TABLE {$alt_table} LIKE {$table}");
      db_query("INSERT INTO {$alt_table} SELECT * FROM {$table}");
      
      $context['results']['tables_created']++;
    }
    catch (Exception $e) {
      backdrop_set_message(t('Error copying table @table: @message', array(
        '@table' => $table,
        '@message' => $e->getMessage(),
      )), 'error');
    }
    
    $context['sandbox']['progress']++;
  }
  
  $context['message'] = t('Copied @progress of @max tables...', array(
    '@progress' => $context['sandbox']['progress'],
    '@max' => $context['sandbox']['max'],
  ));
  
  if ($context['sandbox']['progress'] < $context['sandbox']['max']) {
    $context['finished'] = $context['sandbox']['progress'] / $context['sandbox']['max'];
  }
}

/**
 * Batch operation: Write settings.bluegreen.php configuration.
 */
function bluegreen_setup_write_settings($initial_active, &$context) {
  // Create settings.bluegreen.php file with blue-green configuration
  $settings_content = <<<'PHP'
<?php
/**
 * @file
 * Blue-Green Deployment database configuration.
 * 
 * This file is automatically generated and managed by the bluegreen module.
 * Do not edit manually - use the bluegreen admin interface instead.
 */

// Convert DDEV's $database (singular) to $databases array format if needed
if (isset($database) && is_string($database)) {
  // Parse DDEV's database URL format: mysql://user:pass@host:port/dbname
  $db_url = parse_url($database);
  $base_config = array(
    'driver' => $db_url['scheme'],
    'database' => ltrim($db_url['path'], '/'),
    'username' => $db_url['user'],
    'password' => $db_url['pass'],
    'host' => $db_url['host'],
    'port' => isset($db_url['port']) ? $db_url['port'] : 3306,
    'charset' => 'utf8mb4',
    'collation' => 'utf8mb4_general_ci',
  );
  unset($database); // Remove to prevent Backdrop from using it
}
else {
  // Fallback for production (manual configuration)
  $base_config = array(
    'driver' => 'mysql',
    'database' => 'db',
    'username' => 'db',
    'password' => 'db',
    'host' => 'db',
    'port' => 3306,
    'charset' => 'utf8mb4',
    'collation' => 'utf8mb4_general_ci',
  );
}

// Shared tables that should always use unprefixed versions
$shared_table_prefix = array(
  'default' => '',
  'state' => '',
  'watchdog' => '',
  'users' => '',
  'users_roles' => '',
  'role' => '',
  'role_permission' => '',
  'authmap' => '',
  'sessions' => '',
);

// Set up Blue environment (unprefixed tables - the original/live)
$databases['blue']['default'] = $base_config;
$databases['blue']['default']['prefix'] = $shared_table_prefix;

// Set up Green environment (alt_ prefixed tables - the alternate/dev)
$databases['green']['default'] = $base_config;
$databases['green']['default']['prefix'] = array_merge(
  $shared_table_prefix,
  array('default' => 'alt_')
);

// Set up default database to point to active environment
// This prefix is dynamically updated by the bluegreen module when switching
$databases['default']['default'] = $base_config;
$databases['default']['default']['prefix'] = $shared_table_prefix;

PHP;
  
  $settings_file = BACKDROP_ROOT . '/settings.bluegreen.php';
  
  try {
    $bytes_written = file_put_contents($settings_file, $settings_content);
    if ($bytes_written === FALSE) {
      throw new Exception(t('Could not write settings.bluegreen.php'));
    }
    
    // Also add the include statement to settings.php if not already present
    $settings_php = BACKDROP_ROOT . '/settings.php';
    $settings_php_content = file_get_contents($settings_php);
    
    if (strpos($settings_php_content, 'settings.bluegreen.php') === FALSE) {
      // Add the include after DDEV settings
      $include_code = "\n// Blue-Green Deployment Configuration\n// Managed by the bluegreen module\n\$bluegreen_settings = __DIR__ . '/settings.bluegreen.php';\nif (is_readable(\$bluegreen_settings)) {\n  require \$bluegreen_settings;\n}\n";
      
      // Find the end of the DDEV include section and add after it
      if (strpos($settings_php_content, 'settings.ddev.php') !== FALSE) {
        $settings_php_content = preg_replace(
          '/(require \$ddev_settings;\s*})/s',
          '$1' . $include_code,
          $settings_php_content
        );
      }
      else {
        // No DDEV, add at the end
        $settings_php_content .= $include_code;
      }
      
      file_put_contents($settings_php, $settings_php_content);
    }
    
    $context['results']['settings_written'] = TRUE;
    $context['message'] = t('Created settings.bluegreen.php');
  }
  catch (Exception $e) {
    $context['results']['settings_error'] = $e->getMessage();
    backdrop_set_message(t('Error writing settings file: @message', array('@message' => $e->getMessage())), 'error');
  }
}

/**
 * Batch operation: Finalize setup.
 */
function bluegreen_setup_finalize($initial_active, &$context) {
  // Set the active environment in state system
  state_set('bluegreen_active_environment', $initial_active);
  
  // Clear all caches to recognize new configuration
  backdrop_flush_all_caches();
  
  $context['message'] = t('Finalizing setup...');
  $context['results']['active_environment'] = $initial_active;
}

/**
 * Batch finished callback.
 */
function bluegreen_setup_finished($success, $results, $operations) {
  if ($success) {
    backdrop_set_message(t('Blue-Green deployment has been successfully set up!'));
    backdrop_set_message(t('Created @count tables across both environments.', array(
      '@count' => $results['tables_created'],
    )));
    backdrop_set_message(t('Active environment: @env', array(
      '@env' => $results['active_environment'],
    )));
  }
  else {
    backdrop_set_message(t('An error occurred during setup.'), 'error');
  }
}
