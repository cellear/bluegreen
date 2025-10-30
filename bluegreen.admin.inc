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
    $form['setup_message'] = array(
      '#markup' => '<h2>' . t('Welcome to Blue-Green Deployment') . '</h2>' .
        '<p>' . t('Blue-Green deployment is not configured yet. Use the Setup tab to initialize blue-green deployment for this site.') . '</p>' .
        '<p><a href="@setup_url" class="button button-primary">' . t('Go to Setup') . '</a></p>',
      '#markup_variables' => array(
        '@setup_url' => url('admin/config/development/bluegreen/setup'),
      ),
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

  $form['environments']['blue']['info'] = array(
    '#markup' => '<div class="environment-info">' .
      '<div class="info-line"><strong>' . t('Database:') . '</strong> ' . check_plain($blue_info['database']) . '</div>' .
      '<div class="info-line"><strong>' . t('Prefix:') . '</strong> ' . check_plain(!empty($blue_info['prefix']) ? $blue_info['prefix'] : t('(none)')) . '</div>' .
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

  $form['environments']['green']['info'] = array(
    '#markup' => '<div class="environment-info">' .
      '<div class="info-line"><strong>' . t('Database:') . '</strong> ' . check_plain($green_info['database']) . '</div>' .
      '<div class="info-line"><strong>' . t('Prefix:') . '</strong> ' . check_plain(!empty($green_info['prefix']) ? $green_info['prefix'] : t('(none)')) . '</div>' .
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
    
    // Stay on the bluegreen page to show the updated status
    $form_state['redirect'] = 'admin/config/development/bluegreen';
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

  // Drop all existing tables in target environment
  $target_tables = db_find_tables($to_prefix . '%');
  foreach ($target_tables as $table) {
    db_query("DROP TABLE IF EXISTS {$table}");
  }

  // Copy each table from source to target
  foreach ($source_tables as $source_table) {
    // Determine target table name by replacing prefix
    $base_name = substr($source_table, strlen($from_prefix));
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
  
  // Update settings.php to change the prefix in the default database configuration
  $settings_file = conf_path() . '/settings.php';
  $settings_content = file_get_contents($settings_file);
  $original_content = $settings_content;
  
  // Find the 'default' => array section and update its prefix
  $new_prefix = $env_info['prefix'];
  
  // Use regex to replace the prefix in the 'default' database configuration
  // This matches: 'prefix' => 'anything', within the 'default' => array section
  $pattern = "/(\'default\'\s*=>\s*array\s*\(\s*\'default\'\s*=>\s*array\s*\([^)]*\'prefix\'\s*=>\s*\')([^']*)(\')/s";
  $replacement = '${1}' . $new_prefix . '${3}';
  $settings_content = preg_replace($pattern, $replacement, $settings_content);
  
  // Check if replacement actually happened
  if ($settings_content === $original_content) {
    throw new Exception(t('Failed to update prefix in settings.php. Pattern did not match.'));
  }
  
  // Write back to file
  $bytes_written = file_put_contents($settings_file, $settings_content);
  if ($bytes_written === FALSE) {
    throw new Exception(t('Could not write to settings.php. Please check file permissions.'));
  }
  
  // Ensure file was written
  clearstatcache(TRUE, $settings_file);
  
  watchdog('bluegreen', 'Updated settings.php: changed prefix from @old to @new (@bytes bytes written)', 
    array('@old' => $databases['default']['default']['prefix'], '@new' => $new_prefix, '@bytes' => $bytes_written), 
    WATCHDOG_INFO);
  
  // Use state system to store the active environment
  state_set('bluegreen_active_environment', $new_environment);
  
  // Clear all caches
  backdrop_flush_all_caches();

  // Log the switch
  watchdog('bluegreen', 'Switched environment to @env', 
    array('@env' => $new_environment), WATCHDOG_INFO);
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
 * Form builder for blue-green deployment setup.
 */
function bluegreen_setup_form($form, &$form_state) {
  global $databases;
  
  // Check if already configured
  if (bluegreen_is_configured()) {
    backdrop_set_message(t('Blue-Green deployment is already configured. Use the Overview page to manage environments.'), 'warning');
    $form['message'] = array(
      '#markup' => '<p>' . t('This site is already configured for blue-green deployment.') . '</p>',
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
      '<li>' . t('Duplicating your current database tables with blue_ and green_ prefixes') . '</li>' .
      '<li>' . t('Updating settings.php with environment configurations') . '</li>' .
      '<li>' . t('Setting up the state tracking system') . '</li>' .
      '</ol>',
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
      '<li>' . t('Create duplicate tables (may take several minutes for large databases)') . '</li>' .
      '<li>' . t('Modify your settings.php file (please ensure it is writable)') . '</li>' .
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
  $idle = ($initial_active === 'blue') ? 'green' : 'blue';
  
  // Process up to 10 tables per batch
  $limit = min(10, $context['sandbox']['max'] - $context['sandbox']['progress']);
  $tables_to_process = array_slice($tables, $context['sandbox']['progress'], $limit);
  
  foreach ($tables_to_process as $table) {
    try {
      // Determine the base table name (without current prefix)
      $base_name = $current_prefix ? substr($table, strlen($current_prefix)) : $table;
      
      // Create table names for both environments
      $active_table = $current_prefix . $initial_active . '_' . $base_name;
      $idle_table = $current_prefix . $idle . '_' . $base_name;
      
      // Copy to active environment table
      db_query("CREATE TABLE {$active_table} LIKE {$table}");
      db_query("INSERT INTO {$active_table} SELECT * FROM {$table}");
      
      // Copy to idle environment table
      db_query("CREATE TABLE {$idle_table} LIKE {$table}");
      db_query("INSERT INTO {$idle_table} SELECT * FROM {$table}");
      
      $context['results']['tables_created'] += 2;
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
 * Batch operation: Write settings.php configuration.
 */
function bluegreen_setup_write_settings($initial_active, &$context) {
  global $databases;
  
  $current_db = $databases['default']['default'];
  $current_prefix = !empty($current_db['prefix']) ? $current_db['prefix'] : '';
  
  // Build database configurations
  $blue_config = $current_db;
  $blue_config['prefix'] = $current_prefix . 'blue_';
  $blue_config['driver'] = !empty($current_db['driver']) ? $current_db['driver'] : 'mysql';
  
  $green_config = $current_db;
  $green_config['prefix'] = $current_prefix . 'green_';
  $green_config['driver'] = !empty($current_db['driver']) ? $current_db['driver'] : 'mysql';
  
  // Update default to point to active environment
  $default_config = $current_db;
  $default_config['prefix'] = $current_prefix . $initial_active . '_';
  $default_config['driver'] = !empty($current_db['driver']) ? $current_db['driver'] : 'mysql';
  
  // Load install.inc for backdrop_rewrite_settings()
  require_once BACKDROP_ROOT . '/core/includes/install.inc';
  
  try {
    backdrop_rewrite_settings(array(
      'databases' => array(
        'value' => array(
          'default' => array('default' => $default_config),
          'blue' => array('default' => $blue_config),
          'green' => array('default' => $green_config),
        ),
        'required' => TRUE,
      ),
    ));
    
    $context['results']['settings_written'] = TRUE;
    $context['message'] = t('Updated settings.php');
  }
  catch (Exception $e) {
    $context['results']['settings_error'] = $e->getMessage();
    backdrop_set_message(t('Error writing settings.php: @message', array('@message' => $e->getMessage())), 'error');
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
