#!/usr/bin/env php
<?php
/**
 * Test script for Blue-Green config management functions.
 *
 * Run this from your Backdrop root directory:
 *   php modules/bluegreen/test-config-functions.php
 */

// Bootstrap Backdrop
define('BACKDROP_ROOT', getcwd());
require_once BACKDROP_ROOT . '/core/includes/bootstrap.inc';
backdrop_bootstrap(BACKDROP_FULL);

// Load the bluegreen module
module_load_include('module', 'bluegreen', 'bluegreen');

echo "=== Testing Blue-Green Config Management Functions ===\n\n";

// Test 1: Detect config directory
echo "1. Testing bluegreen_detect_config_directory():\n";
$config_dir = bluegreen_detect_config_directory();
if ($config_dir) {
  echo "   ✓ Found config directory: $config_dir\n";
  echo "   ✓ Directory exists: " . (is_dir($config_dir) ? 'YES' : 'NO') . "\n";
} else {
  echo "   ✗ Could not detect config directory\n";
}
echo "\n";

// Test 2: Get config directory for each environment
echo "2. Testing bluegreen_get_config_directory():\n";
$blue_active = bluegreen_get_config_directory('blue', 'active');
$blue_staging = bluegreen_get_config_directory('blue', 'staging');
$green_active = bluegreen_get_config_directory('green', 'active');
$green_staging = bluegreen_get_config_directory('green', 'staging');

echo "   Blue active:   $blue_active\n";
echo "   Blue staging:  $blue_staging\n";
echo "   Green active:  $green_active\n";
echo "   Green staging: $green_staging\n";
echo "\n";

// Test 3: Count config files
echo "3. Testing bluegreen_count_config_files():\n";
$blue_count = bluegreen_count_config_files('blue', 'active');
$green_count = bluegreen_count_config_files('green', 'active');

echo "   Blue config files:  $blue_count\n";
echo "   Green config files: $green_count\n";
echo "\n";

// Test 4: Check if green config directory exists
echo "4. Checking if Green config directory exists:\n";
if (is_dir($green_active)) {
  echo "   ✓ Green config directory EXISTS\n";
  echo "   (Module is already set up for config management)\n";
} else {
  echo "   ✗ Green config directory DOES NOT EXIST\n";
  echo "   (Will be created when you run setup wizard or call bluegreen_create_alt_config_directory())\n";
}
echo "\n";

// Test 5: Show what would be created
echo "5. What will be created during setup:\n";
$base_config = bluegreen_detect_config_directory();
if ($base_config) {
  $base_path = dirname($base_config);
  $alt_path = $base_path . '_alt';

  echo "   Current config:  $base_path\n";
  echo "   Alternate will be: $alt_path\n";
  echo "   - $alt_path/active/\n";
  echo "   - $alt_path/staging/\n";
}
echo "\n";

echo "=== Testing Complete ===\n";
