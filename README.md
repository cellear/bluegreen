# Blue-Green Deployment Module

Provides blue-green deployment functionality for Backdrop CMS, enabling easy site updates with instant rollback capability.

## Features

- **Dual Database Environments**: Manage separate Blue and Green database environments
- **One-Click Switching**: Switch between environments with a single button click
- **Database Sync**: Sync database content from live to dev environment
- **Visual Interface**: Clean, intuitive admin UI showing environment status
- **Backup Support**: Create backups before switching environments
- **Safe Deployment**: Make changes on idle environment, test, then switch live

## Requirements

- Both Blue and Green database configurations must be defined in `settings.php`
- The `$bluegreen_active_environment` variable must be set in `settings.php`

## Installation

1. Enable the module at `admin/modules`
2. Grant permissions at `admin/people/permissions` (search for "blue-green")
3. Visit `admin/config/development/bluegreen` to manage environments

## Configuration

The module requires database configurations to be set during Backdrop installation using the modified installer, or manually in `settings.php`.

### Important: Driver Field Required

**Each database configuration MUST include a `'driver'` field** to prevent PHP warnings. The Backdrop installer may omit this field for MySQL databases. You must add it manually:

```php
$databases = array(
  'blue' => array(
    'default' => array(
      'database' => 'db',
      'username' => 'db',
      'password' => 'db',
      'host' => 'localhost',
      'driver' => 'mysql',  // REQUIRED - prevents warnings
      'prefix' => 'blue_',
    ),
  ),
  'green' => array(
    'default' => array(
      'database' => 'db',
      'username' => 'db',
      'password' => 'db',
      'host' => 'localhost',
      'driver' => 'mysql',  // REQUIRED - prevents warnings
      'prefix' => 'green_',
    ),
  ),
);
```

You'll also need to ensure `$databases['default']` includes the driver field:

```php
$databases['default']['default']['driver'] = 'mysql';
```

**Why?** Without the `'driver'` field, Backdrop assumes the configuration is an array of multiple database servers (for load balancing) and generates "Undefined array key" warnings when trying to select one randomly.

## Usage

1. **View Environment Status**: Visit the Blue-Green Deployment admin page to see both environments
2. **Sync Database**: Click "Sync from [Active]" on the idle environment to copy the live database
3. **Make Changes**: Work on the idle environment without affecting the live site
4. **Switch Live**: Click "Make [Environment] Live" to instantly switch environments
5. **Rollback**: Switch back just as easily if something goes wrong

## Permissions

- **Administer blue-green deployment**: Full access to all functionality
- **Switch environments**: Can switch which environment is live
- **Sync environments**: Can sync database between environments

## Technical Notes

- Uses Backdrop's database abstraction layer for cross-environment operations
- Integrates with core backup module for pre-switch backups
- Tracks last sync time for each environment
- Validates environment configuration on runtime

## Workflow Example

1. Site is live on Blue environment
2. Sync Blue → Green to get current data
3. Test updates/changes on Green
4. Switch to Green (Green is now live)
5. If problems occur, switch back to Blue instantly

## Support

See the project repository for issues and documentation.

