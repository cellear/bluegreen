# Blue-Green Deployment Module - Project Outline

**Project:** Blue-Green Deployment for Backdrop CMS
**Repository:** https://github.com/cellear/bluegreen
**Purpose:** Dual database environment management for safe deployment and easy rollback
**Generated:** 2025-11-08

---

## Table of Contents

1. [Project Overview](#project-overview)
2. [Code Files by Category](#code-files-by-category)
3. [Function Reference by File](#function-reference-by-file)
4. [Architectural Components](#architectural-components)
5. [Suggestions for Future Enhancements](#suggestions-for-future-enhancements)

---

## Project Overview

**Type:** Backdrop CMS Module (PHP-based)
**Framework:** Backdrop CMS 1.x
**Architecture:** Two-table system with shared tables for critical data

**Key Features:**
- Dual Database Environments (Blue and Green)
- One-Click Environment Switching
- Database Sync between environments
- Configuration directory management
- Visual admin interface
- Backup support
- Shared table architecture for users, sessions, and permissions

---

## Code Files by Category

### Core Module Files (PHP)
1. `bluegreen.module` (593 lines) - Core module functionality
2. `bluegreen.admin.inc` (1106 lines) - Admin UI and form callbacks
3. `bluegreen.install` (90 lines) - Installation and uninstall hooks

### Sample/Example Module Files
4. `mymodule.module` (45 lines) - Example module demonstrating Backdrop patterns
5. `mymodule.admin.inc` (25 lines) - Example admin configuration form
6. `mymodule.install` (19 lines) - Example install hooks

### CLI Scripts
7. `create-config-dirs.php` (56 lines) - Utility to create alternate config directories
8. `test-config-functions.php` (79 lines) - Testing script for config management
9. `setup-ddev.sh` (79 lines) - DDEV environment automation script

### Frontend Assets
10. `css/bluegreen.admin.css` (199 lines) - Admin UI styling
11. `css/mymodule.css` (9 lines) - Sample module CSS
12. `js/mymodule.js` (15 lines) - Sample JavaScript with Backdrop behavior

### Configuration Files
13. `config/mymodule.settings.json` (5 lines) - JSON configuration storage example
14. `ddev-config-snippet.yaml` (14 lines) - DDEV dual-domain setup template

---

## Function Reference by File

### 1. bluegreen.module

Core module file containing environment management, configuration, and database operations.

#### bluegreen_menu()
**Line:** 13
**Description:** Defines menu items for admin interface (overview, setup, switch, sync, backup, cache clear)
**Calls:** *(none - returns array structure)*

#### bluegreen_permission()
**Line:** 84
**Description:** Defines three permissions: administer deployment, switch environments, sync environments
**Calls:** `t()`

#### bluegreen_get_active_environment()
**Line:** 110
**Description:** Returns currently active environment name ('blue' or 'green') from state system
**Calls:** `state_get()`

#### bluegreen_get_environment_info($environment)
**Line:** 124
**Description:** Returns database connection array for specified environment or FALSE if not configured
**Calls:** *(none - accesses global $databases)*

#### bluegreen_is_configured()
**Line:** 140
**Description:** Checks if both blue and green databases are configured and have tables
**Calls:** `bluegreen_get_environment_info()`, `bluegreen_environment_is_installed()`

#### bluegreen_get_idle_environment()
**Line:** 160
**Description:** Returns name of the idle (non-active) environment
**Calls:** `bluegreen_get_active_environment()`

#### bluegreen_get_shared_tables()
**Line:** 174
**Description:** Returns array of table names that should be shared across both environments
**Calls:** *(none - returns static array)*

#### bluegreen_get_last_sync($environment)
**Line:** 196
**Description:** Retrieves unix timestamp of last sync for specified environment
**Calls:** `state_get()`

#### bluegreen_set_last_sync($environment, $timestamp)
**Line:** 208
**Description:** Sets last sync timestamp for environment, defaults to current REQUEST_TIME
**Calls:** `state_set()`

#### bluegreen_get_table_count($environment)
**Line:** 224
**Description:** Counts number of tables in specified environment by connecting and querying schema
**Calls:** `bluegreen_get_environment_info()`, `Database::addConnectionInfo()`, `Database::setActiveConnection()`, `Database::getConnection()`, `Database::removeConnection()`, `watchdog()`

#### bluegreen_environment_is_installed($environment)
**Line:** 289
**Description:** Returns TRUE if environment has tables (table count > 0)
**Calls:** `bluegreen_get_table_count()`

#### bluegreen_detect_config_directory()
**Line:** 302
**Description:** Finds and returns path to Backdrop's active config directory
**Calls:** `config_get_config_directory()`, `is_dir()`, `scandir()`

#### bluegreen_get_config_directory($environment, $type)
**Line:** 333
**Description:** Returns config directory path for environment, appending _alt suffix for green
**Calls:** `bluegreen_detect_config_directory()`, `dirname()`

#### bluegreen_count_config_files($environment, $type)
**Line:** 362
**Description:** Counts JSON config files in environment's config directory
**Calls:** `bluegreen_get_config_directory()`, `is_dir()`, `glob()`

#### bluegreen_copy_directory($source, $destination)
**Line:** 386
**Description:** Recursively copies all files and subdirectories from source to destination
**Calls:** `is_dir()`, `mkdir()`, `scandir()`, `bluegreen_copy_directory()` *(recursive)*, `copy()`, `chmod()`

#### bluegreen_sync_config_files($from, $to)
**Line:** 439
**Description:** Syncs both active and staging config files between environments
**Calls:** `bluegreen_get_config_directory()`, `bluegreen_copy_directory()`, `is_dir()`, `watchdog()`

#### bluegreen_create_alt_config_directory()
**Line:** 489
**Description:** Creates alternate config directory (_alt) for green environment during setup
**Calls:** `bluegreen_detect_config_directory()`, `is_dir()`, `mkdir()`, `bluegreen_copy_directory()`, `watchdog()`

#### bluegreen_init()
**Line:** 549
**Description:** Implements hook_init to add admin CSS when on bluegreen admin pages
**Calls:** `arg()`, `backdrop_add_css()`, `backdrop_get_path()`

#### bluegreen_admin_bar_output_build(&$content)
**Line:** 559
**Description:** Implements hook to add environment indicator (Blue/Green) to admin bar
**Calls:** `bluegreen_is_configured()`, `bluegreen_get_active_environment()`, `backdrop_add_css()`, `backdrop_get_path()`

---

### 2. bluegreen.admin.inc

Admin UI, forms, callbacks, and batch operations for environment management.

#### bluegreen_admin_form($form, &$form_state)
**Line:** 10
**Description:** Builds main admin page showing dual environment panels with status and actions
**Calls:** `bluegreen_is_configured()`, `backdrop_set_message()`, `url()`, `bluegreen_get_active_environment()`, `bluegreen_get_idle_environment()`, `bluegreen_get_environment_info()`, `bluegreen_environment_is_installed()`, `bluegreen_get_last_sync()`, `check_url()`, `check_plain()`, `format_date()`, `module_exists()`

#### bluegreen_backup_submit($form, &$form_state)
**Line:** 248
**Description:** Submit handler that triggers backup creation for specified environment
**Calls:** `module_load_include()`, `backup_create_backup()`, `backdrop_set_message()`

#### bluegreen_sync_submit($form, &$form_state)
**Line:** 274
**Description:** Submit handler that sets up batch operation for syncing environments
**Calls:** `batch_set()`, `backdrop_get_path()`

#### bluegreen_sync_batch_operation($from, $to, &$context)
**Line:** 295
**Description:** Batch operation that syncs database tables and config files between environments
**Calls:** `bluegreen_sync_database()`, `bluegreen_sync_config_files()`, `bluegreen_set_last_sync()`

#### bluegreen_sync_batch_finished($success, $results, $operations)
**Line:** 329
**Description:** Batch finished callback that displays sync results and any errors
**Calls:** `backdrop_set_message()`

#### bluegreen_switch_submit($form, &$form_state)
**Line:** 354
**Description:** Submit handler that switches active environment and redirects to cache clear
**Calls:** `bluegreen_get_active_environment()`, `bluegreen_get_idle_environment()`, `bluegreen_switch_environment()`, `backdrop_set_message()`

#### bluegreen_sync_database($from, $to)
**Line:** 387
**Description:** Copies all non-shared tables from source to target environment
**Calls:** `bluegreen_get_environment_info()`, `db_find_tables()`, `bluegreen_get_shared_tables()`, `db_query()`

#### bluegreen_copy_table($from_info, $to_info, $from_table, $to_table)
**Line:** 445
**Description:** Copies single table structure and data between environments using CREATE LIKE and INSERT
**Calls:** `Database::addConnectionInfo()`, `Database::setActiveConnection()`, `Database::getConnection()`, `Database::removeConnection()`

#### bluegreen_switch_environment($new_environment)
**Line:** 506
**Description:** Switches active environment by updating settings.bluegreen.php and state system
**Calls:** `bluegreen_get_environment_info()`, `file_exists()`, `file_get_contents()`, `preg_replace()`, `file_put_contents()`, `clearstatcache()`, `watchdog()`, `state_set()`

#### bluegreen_switch_environment_callback($environment)
**Line:** 587
**Description:** Menu callback that switches environment and redirects to admin page
**Calls:** `bluegreen_switch_environment()`, `backdrop_set_message()`, `backdrop_goto()`

#### bluegreen_sync_environment_callback($from, $to)
**Line:** 603
**Description:** Menu callback that syncs database and config between environments
**Calls:** `bluegreen_sync_database()`, `bluegreen_sync_config_files()`, `bluegreen_set_last_sync()`, `backdrop_set_message()`, `backdrop_goto()`

#### bluegreen_backup_environment_callback($environment)
**Line:** 636
**Description:** Menu callback that creates backup and redirects to admin page
**Calls:** `module_load_include()`, `backup_create_backup()`, `backdrop_set_message()`, `backdrop_goto()`

#### bluegreen_clear_cache_callback()
**Line:** 659
**Description:** Menu callback that clears all caches after environment switch
**Calls:** `cache_clear_all()`, `backdrop_flush_all_caches()`, `backdrop_set_message()`, `backdrop_goto()`

#### bluegreen_setup_form($form, &$form_state)
**Line:** 676
**Description:** Form builder for initial setup wizard showing current config and confirmation
**Calls:** `bluegreen_is_configured()`, `check_plain()`, `db_find_tables()`

#### bluegreen_setup_form_submit($form, &$form_state)
**Line:** 763
**Description:** Submit handler that sets up batch for table duplication and config creation
**Calls:** `batch_set()`, `backdrop_get_path()`

#### bluegreen_setup_duplicate_tables($initial_active, &$context)
**Line:** 785
**Description:** Batch operation that creates alt_ prefixed tables for green environment
**Calls:** `db_find_tables()`, `bluegreen_get_shared_tables()`, `db_query()`, `backdrop_set_message()`

#### bluegreen_setup_create_config_directories($initial_active, &$context)
**Line:** 859
**Description:** Batch operation that creates alternate config directories with file copies
**Calls:** `bluegreen_create_alt_config_directory()`, `bluegreen_count_config_files()`, `backdrop_set_message()`

#### bluegreen_setup_write_settings($initial_active, &$context)
**Line:** 889
**Description:** Batch operation that writes settings.bluegreen.php with dual-domain routing
**Calls:** `file_put_contents()`, `file_get_contents()`, `preg_replace()`, `backdrop_set_message()`

#### bluegreen_setup_finalize($initial_active, &$context)
**Line:** 1070
**Description:** Batch operation that sets active environment and clears all caches
**Calls:** `state_set()`, `backdrop_flush_all_caches()`

#### bluegreen_setup_finished($success, $results, $operations)
**Line:** 1084
**Description:** Batch finished callback that displays setup completion messages
**Calls:** `backdrop_set_message()`

---

### 3. bluegreen.install

Installation, requirements checking, and uninstallation hooks.

#### bluegreen_requirements($phase)
**Line:** 10
**Description:** Checks if both blue and green environments are configured at runtime
**Calls:** `get_t()`

#### bluegreen_install()
**Line:** 52
**Description:** Displays message with link to configuration page when module is enabled
**Calls:** `backdrop_set_message()`, `url()`

#### bluegreen_uninstall()
**Line:** 60
**Description:** Removes state variables, drops all alt_ tables, and deletes settings.bluegreen.php
**Calls:** `state_del()`, `db_find_tables()`, `db_query()`, `watchdog()`, `file_exists()`, `unlink()`

---

### 4. mymodule.module

Example module demonstrating Backdrop CMS patterns and best practices.

#### mymodule_config_info()
**Line:** 10
**Description:** Defines configuration schema for mymodule.settings config file
**Calls:** `t()`

#### mymodule_menu()
**Line:** 22
**Description:** Defines admin configuration page menu item
**Calls:** *(none - returns array)*

#### mymodule_permission()
**Line:** 38
**Description:** Defines 'administer mymodule' permission
**Calls:** `t()`

---

### 5. mymodule.admin.inc

Example admin configuration form.

#### mymodule_configuration_form($form, &$form_state)
**Line:** 10
**Description:** Builds configuration form with textfield for storing opinion of Backdrop
**Calls:** `config()`, `system_settings_form()`

---

### 6. mymodule.install

Example install hooks showing Drupal 7 to Backdrop migration.

#### mymodule_update_1000()
**Line:** 10
**Description:** Migrates Drupal 7 variables to Backdrop config system
**Calls:** `config()`, `update_variable_get()`, `update_variable_del()`

---

### 7. create-config-dirs.php

CLI script (56 lines) - No functions defined. Executable script that bootstraps Backdrop and calls:
- `backdrop_bootstrap()`
- `module_load_include()`
- `bluegreen_create_alt_config_directory()`
- `bluegreen_count_config_files()`
- `bluegreen_get_config_directory()`
- `is_dir()`

**Purpose:** Command-line utility to create alternate config directories for existing installations.

---

### 8. test-config-functions.php

CLI script (79 lines) - No functions defined. Executable script that tests config management functions:
- `backdrop_bootstrap()`
- `module_load_include()`
- `bluegreen_detect_config_directory()`
- `bluegreen_get_config_directory()`
- `bluegreen_count_config_files()`
- `is_dir()`
- `dirname()`

**Purpose:** Testing and validation script for config directory detection and management.

---

### 9. setup-ddev.sh

Bash script (79 lines) - No PHP functions. Shell script that:
- Configures DDEV project for Backdrop CMS
- Adds dual-domain support (main and dev subdomains)
- Starts DDEV environment
- Displays setup instructions

**Purpose:** Automates DDEV environment setup with dual-domain configuration.

---

### 10. css/bluegreen.admin.css

Stylesheet (199 lines) - No functions. CSS file containing styles for:
- `.bluegreen-environments-wrapper` - Flex container for dual panels
- `.bluegreen-environment` - Individual environment panels
- `.bluegreen-blue`, `.bluegreen-green` - Environment-specific colors
- `.environment-status` - Status badges (Live/Idle)
- `.environment-info` - Information sections
- `.environment-actions` - Action button containers
- `.bluegreen-switch-section` - Switch action area
- `#admin-bar-environment` - Admin toolbar indicator
- Responsive styles (mobile breakpoint: 768px)

**Purpose:** Visual styling for blue-green deployment admin interface.

---

### 11. css/mymodule.css

Stylesheet (9 lines) - Sample CSS demonstrating basic styling.

---

### 12. js/mymodule.js

JavaScript (15 lines)

#### Backdrop.behaviors.myModule.attach(context, settings)
**Line:** 8
**Description:** Backdrop behavior that logs "Hello world!" to browser console
**Calls:** `console.log()`

**Purpose:** Example JavaScript demonstrating Backdrop.behaviors pattern.

---

## Architectural Components

### Data Flow: Environment Switch

```
User clicks "Make Green Live"
  ↓
bluegreen_switch_submit()
  ↓
bluegreen_switch_environment('green')
  ├─ Updates settings.bluegreen.php (changes prefix)
  ├─ Calls state_set('bluegreen_active_environment', 'green')
  └─ Logs to watchdog
  ↓
Redirect to bluegreen_clear_cache_callback()
  ├─ Clears admin bar cache
  ├─ Calls backdrop_flush_all_caches()
  └─ Redirects to admin page
  ↓
New environment is live
```

### Data Flow: Database Sync

```
User clicks "Sync from Blue"
  ↓
bluegreen_sync_submit()
  ↓
batch_set() → bluegreen_sync_batch_operation()
  ├─ bluegreen_sync_database('blue', 'green')
  │   ├─ Gets table lists with db_find_tables()
  │   ├─ Drops existing target tables (except shared)
  │   ├─ Creates new tables with CREATE TABLE LIKE
  │   └─ Copies data with INSERT INTO SELECT
  │
  ├─ bluegreen_sync_config_files('blue', 'green')
  │   ├─ Gets config directory paths
  │   └─ Calls bluegreen_copy_directory() for active and staging
  │
  └─ bluegreen_set_last_sync('green', REQUEST_TIME)
  ↓
bluegreen_sync_batch_finished()
  └─ Displays success messages
```

### Hook Implementation Map

| Hook | Implemented In | Function Name |
|------|----------------|---------------|
| hook_menu | bluegreen.module:13 | bluegreen_menu() |
| hook_permission | bluegreen.module:84 | bluegreen_permission() |
| hook_init | bluegreen.module:549 | bluegreen_init() |
| hook_admin_bar_output_build | bluegreen.module:559 | bluegreen_admin_bar_output_build() |
| hook_requirements | bluegreen.install:10 | bluegreen_requirements() |
| hook_install | bluegreen.install:52 | bluegreen_install() |
| hook_uninstall | bluegreen.install:60 | bluegreen_uninstall() |
| hook_config_info | mymodule.module:10 | mymodule_config_info() |
| hook_menu | mymodule.module:22 | mymodule_menu() |
| hook_permission | mymodule.module:38 | mymodule_permission() |

### Menu Callbacks Map

| Path | Callback Function | File |
|------|------------------|------|
| admin/content/bluegreen | backdrop_get_form('bluegreen_admin_form') | bluegreen.admin.inc |
| admin/content/bluegreen/overview | *(default task)* | bluegreen.admin.inc |
| admin/content/bluegreen/setup | backdrop_get_form('bluegreen_setup_form') | bluegreen.admin.inc |
| admin/content/bluegreen/switch/% | bluegreen_switch_environment_callback() | bluegreen.admin.inc |
| admin/content/bluegreen/sync/%/% | bluegreen_sync_environment_callback() | bluegreen.admin.inc |
| admin/content/bluegreen/backup/% | bluegreen_backup_environment_callback() | bluegreen.admin.inc |
| admin/content/bluegreen/clear-cache | bluegreen_clear_cache_callback() | bluegreen.admin.inc |
| admin/config/system/mymodule | backdrop_get_form('mymodule_configuration_form') | mymodule.admin.inc |

### Shared Tables Architecture

The following tables exist only once and are shared across both environments:

- `state` - System state tracking (prevents loss of active environment setting)
- `watchdog` - Audit log (single timeline across switches)
- `users` - User accounts (prevents password rollbacks)
- `users_roles` - Role assignments
- `role` - Role definitions
- `role_permission` - Permission settings
- `authmap` - External authentication
- `sessions` - Active sessions (keeps users logged in during switch)

### Configuration Directory Structure

```
files/config_[hash]/
  ├── active/       # Blue environment active config
  │   └── *.json
  └── staging/      # Blue environment staging config
      └── *.json

files/config_[hash]_alt/
  ├── active/       # Green environment active config
  │   └── *.json
  └── staging/      # Green environment staging config
      └── *.json
```

### Database Prefix Configuration

**Blue Environment (unprefixed):**
```php
$databases['blue']['default']['prefix'] = array(
  'default' => '',
  'state' => '',
  'watchdog' => '',
  'users' => '',
  // ... other shared tables
);
```

**Green Environment (alt_ prefixed):**
```php
$databases['green']['default']['prefix'] = array(
  'default' => 'alt_',
  'state' => '',
  'watchdog' => '',
  'users' => '',
  // ... other shared tables
);
```

---

## Suggestions for Future Enhancements

Based on analyzing the codebase, here are suggestions for additional features that could be included in a project outline:

### 1. **Call Graph Visualization**
- Visual diagram showing which functions call which
- Identify circular dependencies
- Highlight critical path functions

### 2. **Dependency Analysis**
- External module dependencies (backup module, admin_bar, etc.)
- Required PHP version and extensions
- Database driver requirements
- Backdrop CMS version compatibility

### 3. **Entry Points Documentation**
- All hook implementations with line numbers
- Menu callbacks and their paths
- Form submit handlers
- Batch operations
- AJAX callbacks (if any)

### 4. **Data Model Documentation**
- State system keys used (`bluegreen_active_environment`, `bluegreen_last_sync_*`)
- Configuration files structure
- Database table prefixing scheme
- Shared vs. environment-specific tables

### 5. **Security Analysis**
- Permission checks in each callback
- Input validation and sanitization
- SQL injection prevention
- XSS protection (check_plain, check_url usage)

### 6. **Test Coverage Map**
- Unit test files (if they exist)
- Integration tests
- Manual testing procedures
- Edge cases to test

### 7. **Error Handling Patterns**
- Exception handling locations
- Watchdog logging points
- User-facing error messages
- Rollback mechanisms

### 8. **Performance Considerations**
- Batch processing usage
- Large dataset handling (table copying)
- Cache clearing strategies
- Database connection management

### 9. **Configuration Options**
- Settable parameters
- Default values
- Configuration file locations
- Environment variables

### 10. **API Documentation**
- Public vs. private functions
- Function parameter types
- Return value documentation
- Example usage

### 11. **Internationalization (i18n)**
- All translatable strings (t() usage)
- Language support
- Text that needs translation

### 12. **Code Quality Metrics**
- Lines of code per file
- Cyclomatic complexity
- Function length
- Code duplication

### 13. **Development Workflow**
- Git branch strategy
- Commit message conventions
- Pull request process
- Release procedures

### 14. **Domain Logic Map**
- Business rules implementation
- Workflow state machines
- Validation rules
- Data transformation logic

### 15. **Integration Points**
- Backdrop Core API usage
- Database abstraction layer
- Form API
- Batch API
- State system
- Configuration management
- Admin bar integration

---

## Project Statistics

| Metric | Count |
|--------|-------|
| Total PHP Files | 8 |
| Total Functions | 47 |
| Lines of PHP Code | ~2,300 |
| CSS Files | 2 |
| JavaScript Files | 1 |
| Shell Scripts | 1 |
| Hook Implementations | 10 |
| Menu Callbacks | 7 |
| Batch Operations | 5 |
| State Variables | 3 |
| Shared Tables | 8 |

---

**End of Project Outline**
