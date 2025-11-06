#!/usr/bin/env php
<?php
/**
 * Create alternate config directories for existing bluegreen installations.
 *
 * Run this from your Backdrop root directory:
 *   php modules/bluegreen/create-config-dirs.php
 */

// Bootstrap Backdrop
define('BACKDROP_ROOT', getcwd());
require_once BACKDROP_ROOT . '/core/includes/bootstrap.inc';
backdrop_bootstrap(BACKDROP_BOOTSTRAP_FULL);

// Load the bluegreen module
module_load_include('module', 'bluegreen', 'bluegreen');

echo "=== Creating Alternate Config Directories ===\n\n";

try {
  // Call the helper function to create config directories
  bluegreen_create_alt_config_directory();

  echo "✓ Success!\n\n";

  // Show what was created
  echo "Checking results:\n";
  $blue_active = bluegreen_count_config_files('blue', 'active');
  $blue_staging = bluegreen_count_config_files('blue', 'staging');
  $green_active = bluegreen_count_config_files('green', 'active');
  $green_staging = bluegreen_count_config_files('green', 'staging');

  echo "  Blue active:   $blue_active files\n";
  echo "  Blue staging:  $blue_staging files\n";
  echo "  Green active:  $green_active files\n";
  echo "  Green staging: $green_staging files\n\n";

  // Verify directories exist
  $green_active_path = bluegreen_get_config_directory('green', 'active');
  $green_staging_path = bluegreen_get_config_directory('green', 'staging');

  if (is_dir($green_active_path)) {
    echo "✓ Green active directory created: $green_active_path\n";
  }
  if (is_dir($green_staging_path)) {
    echo "✓ Green staging directory created: $green_staging_path\n";
  }

  echo "\n=== Complete ===\n";
  echo "Next step: Update settings.bluegreen.php to include config routing\n";

} catch (Exception $e) {
  echo "✗ Error: " . $e->getMessage() . "\n";
  exit(1);
}
