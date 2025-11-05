# Config File Management - Implementation Plan

## Branch: `feature/config-file-management`

## Overview

Implement configuration file management for blue-green deployment, ensuring that each environment maintains its own set of Backdrop configuration files that can be synced and switched along with the database.

## Background: Backdrop Config System

Backdrop CMS uses a JSON-based configuration management system with two key directories:

- **Active Config** (`files/config_[hash]/active/`): Currently running configuration
- **Staging Config** (`files/config_[hash]/staging/`): For importing/exporting config changes

Configuration files include:
- Core settings (system.core.json, system.date.json, etc.)
- Module configurations
- Views, content types, fields
- Theme settings
- And more...

## Current Problem

Currently, the blue-green module only syncs database tables. When you:
1. Make config changes on the dev environment (Green)
2. Switch to make Green live

**Problem:** Config changes are already live because both environments share the same `files/config_*` directory!

This defeats the purpose of testing changes in isolation before going live.

## Proposed Solution

### Architecture: Dual Config Directories

Create separate config directories for each environment:

```
files/
├── config_[hash]/              # Blue environment (active)
│   ├── active/
│   └── staging/
└── config_[hash]_alt/          # Green environment (alternate)
    ├── active/
    └── staging/
```

### Implementation Strategy

#### 1. Setup Phase (Initial Configuration)

**When setup wizard runs:**
- Detect current config directory location
- Create alternate config directory (`config_[hash]_alt/`)
- Copy all config files from Blue → Green
- Update `settings.bluegreen.php` to include config directory routing

**Files to modify:**
- `bluegreen.admin.inc` - Add config setup to setup wizard
- `settings.bluegreen.php` - Add config directory switching logic

#### 2. Config Directory Routing

**In `settings.bluegreen.php`:**

```php
// Determine which config directory to use based on active environment
$active_env = state_get('bluegreen_active_environment', 'blue');

// For domain-based routing
if (isset($_SERVER['HTTP_HOST'])) {
  if (strpos($_SERVER['HTTP_HOST'], 'dev.') === 0) {
    // Dev domain → idle environment
    $config_env = ($active_env === 'blue') ? 'green' : 'blue';
  } else {
    // Main domain → active environment
    $config_env = $active_env;
  }
}

// Set config directory based on environment
if ($config_env === 'blue') {
  $config_directories['active'] = 'files/config_[hash]/active';
  $config_directories['staging'] = 'files/config_[hash]/staging';
} else {
  $config_directories['active'] = 'files/config_[hash]_alt/active';
  $config_directories['staging'] = 'files/config_[hash]_alt/staging';
}
```

#### 3. Sync Operation

**When syncing from Blue → Green (or vice versa):**

1. Sync database tables (existing functionality)
2. **NEW:** Sync config files:
   - Copy all JSON files from source config/active → target config/active
   - Copy all JSON files from source config/staging → target config/staging
   - Preserve file permissions
   - Log count of files synced

**Add to `bluegreen_sync_database()`:**
```php
// After database sync completes...
bluegreen_sync_config_files($from, $to);
```

**New function:**
```php
function bluegreen_sync_config_files($from, $to) {
  // Get config directory paths for both environments
  $from_config_path = bluegreen_get_config_directory($from);
  $to_config_path = bluegreen_get_config_directory($to);

  // Copy active config
  $active_count = bluegreen_copy_directory(
    $from_config_path . '/active',
    $to_config_path . '/active'
  );

  // Copy staging config
  $staging_count = bluegreen_copy_directory(
    $from_config_path . '/staging',
    $to_config_path . '/staging'
  );

  watchdog('bluegreen', 'Synced @total config files from @from to @to (@active active, @staging staging)',
    array(
      '@total' => $active_count + $staging_count,
      '@from' => $from,
      '@to' => $to,
      '@active' => $active_count,
      '@staging' => $staging_count,
    ));

  return array('active' => $active_count, 'staging' => $staging_count);
}
```

#### 4. Switch Operation

**When switching environments:**

Config directories are automatically routed via `settings.bluegreen.php` - no additional action needed!

The config routing logic reads the state variable and routes to the appropriate directory.

#### 5. UI Updates

**Admin page should display:**
- Config directory path for each environment
- Number of config files in each environment
- Include config file counts in sync feedback

**Example display:**
```
Blue Environment
├─ Status: Live
├─ Database: db (no prefix)
├─ Config: files/config_abc123/active (127 files)
└─ Last synced: Never
```

## Implementation Phases

### Phase 1: Basic Config Directory Support ✅
- [ ] Create helper function to detect config directory location
- [ ] Create helper function to get config directory for environment
- [ ] Add config directory info to admin UI display
- [ ] Document config directory structure

### Phase 2: Setup Wizard Integration ✅
- [ ] Detect current config directory during setup
- [ ] Create alternate config directory (`_alt` suffix)
- [ ] Copy all config files from Blue → Green
- [ ] Generate config routing code in `settings.bluegreen.php`
- [ ] Test setup creates directories correctly

### Phase 3: Sync Integration ✅
- [ ] Implement `bluegreen_copy_directory()` helper
- [ ] Implement `bluegreen_sync_config_files()`
- [ ] Integrate config sync into existing sync operation
- [ ] Add config file counts to sync success message
- [ ] Test config syncing between environments

### Phase 4: Switch Integration ✅
- [ ] Verify config routing in `settings.bluegreen.php` works
- [ ] Test switching environments routes to correct config
- [ ] Clear config cache after switch
- [ ] Test config changes stay isolated per environment

### Phase 5: Testing & Validation ✅
- [ ] Test: Make config change on Green, verify not visible on Blue
- [ ] Test: Switch to Green, verify config change now visible
- [ ] Test: Sync Blue → Green, verify config copied correctly
- [ ] Test: Works with both domain-based and manual switching
- [ ] Document config management workflow

## Files to Modify

**Core Module Files:**
- `bluegreen.admin.inc` - Add config sync, display config info
- `bluegreen.module` - Add helper functions
- `bluegreen.install` - Add config directory cleanup to uninstall

**Configuration Template:**
- `settings.bluegreen.php` (generated) - Add config routing logic

**New Helper Functions Needed:**
```php
bluegreen_get_config_directory($environment)
bluegreen_detect_config_directory()
bluegreen_copy_directory($source, $destination)
bluegreen_sync_config_files($from, $to)
bluegreen_count_config_files($environment)
```

**Documentation:**
- `README.md` - Update with config management info
- `DOCS/CONFIG-MANAGEMENT.md` - Detailed config management guide
- `DOCS/BURNDOWN.md` - Mark task #14 as complete

## Testing Workflow

### Manual Testing Steps:

1. **Initial Setup:**
   ```bash
   # Run setup wizard
   # Verify both config directories created
   ls -la files/config_*/
   ```

2. **Test Isolation:**
   ```bash
   # Visit dev domain
   # Make a config change (e.g., change site name)
   # Visit main domain - verify change NOT visible
   ```

3. **Test Sync:**
   ```bash
   # Make config change on Blue
   # Sync Blue → Green
   # Visit dev domain - verify change IS visible
   ```

4. **Test Switch:**
   ```bash
   # Make config change on Green (dev)
   # Switch to make Green live
   # Visit main domain - verify change IS visible
   ```

## Risks & Considerations

### Risk 1: Performance
**Issue:** Copying config files during sync could be slow for large sites
**Mitigation:** Most sites have < 500 config files, copy is typically fast

### Risk 2: Config Directory Hash
**Issue:** Backdrop uses hash in directory name, need to preserve or detect
**Mitigation:** Detect existing directory, use same hash with `_alt` suffix

### Risk 3: File Permissions
**Issue:** Copied config files need correct permissions
**Mitigation:** Use Backdrop's file system functions which handle permissions

### Risk 4: State Variables
**Issue:** State variables are shared (stored in database `state` table)
**Mitigation:** This is intentional - state tracking active environment must be shared

### Risk 5: Uninstall Cleanup
**Issue:** Must remove alternate config directory on uninstall
**Mitigation:** Add to existing uninstall hook, log which directory was removed

## Success Criteria

✅ Config changes made on dev environment stay isolated until switch
✅ Config syncs correctly from one environment to another
✅ Environment switching correctly routes to appropriate config directory
✅ Setup wizard creates and configures alternate config directory
✅ Uninstall removes alternate config directory
✅ Admin UI shows config directory info for each environment
✅ Works seamlessly with existing database sync functionality

## Timeline Estimate

- **Phase 1:** 1-2 hours (helper functions, UI display)
- **Phase 2:** 2-3 hours (setup wizard integration, settings generation)
- **Phase 3:** 2-3 hours (sync implementation, testing)
- **Phase 4:** 1 hour (switch testing, cache clearing)
- **Phase 5:** 2 hours (comprehensive testing, documentation)

**Total:** 8-11 hours of development + testing

## Questions to Answer

1. **Should we sync staging config too, or only active?**
   - Recommendation: Sync both for consistency

2. **What happens if config directories already exist?**
   - Recommendation: Detect and use existing, or prompt user

3. **Should we support custom config directory locations?**
   - Recommendation: Phase 2 feature, start with standard locations

4. **How to handle config export/import operations?**
   - Recommendation: Document that exports should be done per-environment

## Next Steps

1. Review and approve this plan
2. Start with Phase 1 (helper functions + UI)
3. Incremental commits per phase
4. Test each phase before moving to next
5. Update documentation as we go

---

**Ready to start implementation?** Let me know if you want to adjust the plan or if we should proceed with Phase 1!
