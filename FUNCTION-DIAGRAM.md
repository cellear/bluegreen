# Blue-Green Module - Function Call Diagram

This diagram shows all functions in `bluegreen.module` and their call relationships.

## Interactive Call Graph

```mermaid
graph TD
    %% Styling
    classDef hookFunc fill:#e1f5ff,stroke:#0066cc,stroke-width:2px
    classDef envFunc fill:#fff4e1,stroke:#ff9900,stroke-width:2px
    classDef configFunc fill:#e8f5e9,stroke:#4caf50,stroke-width:2px
    classDef stateFunc fill:#f3e5f5,stroke:#9c27b0,stroke-width:2px

    subgraph " "
        subgraph "Hooks & UI"
            menu[bluegreen_menu :13]:::hookFunc
            perm[bluegreen_permission :84]:::hookFunc
            init[bluegreen_init :549]:::hookFunc
            adminbar[bluegreen_admin_bar_output_build :559]:::hookFunc
        end

        subgraph "Environment Management"
            get_active[get_active_environment :110]:::envFunc
            get_idle[get_idle_environment :160]:::envFunc
            is_configured[is_configured :140]:::envFunc
            env_installed[environment_is_installed :289]:::envFunc
            get_env_info[get_environment_info :124]:::envFunc
            get_table_count[get_table_count :224]:::envFunc
            get_shared[get_shared_tables :174]:::envFunc
        end

        subgraph "Configuration Management"
            detect_config[detect_config_directory :302]:::configFunc
            get_config_dir[get_config_directory :333]:::configFunc
            count_config[count_config_files :362]:::configFunc
            copy_dir[copy_directory :386]:::configFunc
            sync_config[sync_config_files :439]:::configFunc
            create_alt_config[create_alt_config_directory :489]:::configFunc
        end

        subgraph "State Management"
            get_last_sync[get_last_sync :196]:::stateFunc
            set_last_sync[set_last_sync :208]:::stateFunc
        end
    end

    %% Hook to Environment calls
    adminbar --> is_configured
    adminbar --> get_active

    %% Environment function calls
    get_idle --> get_active
    is_configured --> get_env_info
    is_configured --> env_installed
    env_installed --> get_table_count
    get_table_count --> get_env_info

    %% Config function calls
    get_config_dir --> detect_config
    count_config --> get_config_dir
    sync_config --> get_config_dir
    sync_config --> copy_dir
    create_alt_config --> detect_config
    create_alt_config --> copy_dir
    copy_dir -.recursive.-> copy_dir
```

## Environment Functions Call Flow

```mermaid
graph TD
    A[adminbar :559]:::hookFunc
    B[is_configured :140]:::envFunc
    C[get_env_info :124]:::envFunc
    D[env_installed :289]:::envFunc
    E[get_table_count :224]:::envFunc
    F[get_active :110]:::envFunc
    G[get_idle :160]:::envFunc

    A --> B
    A --> F
    B --> C
    B --> D
    D --> E
    E --> C
    G --> F

    classDef hookFunc fill:#e1f5ff,stroke:#0066cc,stroke-width:2px
    classDef envFunc fill:#fff4e1,stroke:#ff9900,stroke-width:2px
```

## Configuration Functions Call Flow

```mermaid
graph TD
    A[sync_config_files :439]:::configFunc
    B[create_alt_config_directory :489]:::configFunc
    C[count_config_files :362]:::configFunc
    D[get_config_directory :333]:::configFunc
    E[detect_config_directory :302]:::configFunc
    F[copy_directory :386]:::configFunc

    A --> D
    A --> F
    B --> E
    B --> F
    C --> D
    D --> E
    F -.recursive.-> F

    classDef configFunc fill:#e8f5e9,stroke:#4caf50,stroke-width:2px
```

## Function Groups

### 🔵 Hook Implementations (Blue)
Functions that implement Backdrop CMS hooks:
- **bluegreen_menu** (:13) - Defines admin pages and menu items
- **bluegreen_permission** (:84) - Defines access permissions
- **bluegreen_init** (:549) - Loads CSS on admin pages
- **bluegreen_admin_bar_output_build** (:559) - Adds environment indicator to admin bar

### 🟠 Environment Functions (Orange)
Functions that manage environment information and state:
- **bluegreen_get_active_environment** (:110) - Returns active env name
- **bluegreen_get_environment_info** (:124) - Gets database config
- **bluegreen_is_configured** (:140) - Checks if setup complete
- **bluegreen_get_idle_environment** (:160) - Returns idle env name
- **bluegreen_get_shared_tables** (:174) - Lists shared tables
- **bluegreen_get_last_sync** (:196) - Gets last sync timestamp
- **bluegreen_set_last_sync** (:208) - Sets sync timestamp
- **bluegreen_get_table_count** (:224) - Counts tables in env
- **bluegreen_environment_is_installed** (:289) - Checks if env has tables

### 🟢 Configuration Functions (Green)
Functions that manage configuration directories:
- **bluegreen_detect_config_directory** (:302) - Finds active config dir
- **bluegreen_get_config_directory** (:333) - Gets env config path
- **bluegreen_count_config_files** (:362) - Counts config files
- **bluegreen_copy_directory** (:386) - Recursively copies directories
- **bluegreen_sync_config_files** (:439) - Syncs config between envs
- **bluegreen_create_alt_config_directory** (:489) - Creates green config dir

### 🟣 State Functions (Purple)
Functions that manage state persistence:
- **bluegreen_get_last_sync** (:196) - Gets last sync timestamp
- **bluegreen_set_last_sync** (:208) - Sets sync timestamp

## Call Chains

### Longest Call Chains

**Admin Bar Display:**
```
bluegreen_admin_bar_output_build
  └─> bluegreen_is_configured
      ├─> bluegreen_get_environment_info
      └─> bluegreen_environment_is_installed
          └─> bluegreen_get_table_count
              └─> bluegreen_get_environment_info
```

**Config File Syncing:**
```
bluegreen_sync_config_files
  ├─> bluegreen_get_config_directory
  │   └─> bluegreen_detect_config_directory
  └─> bluegreen_copy_directory
      └─> bluegreen_copy_directory (recursive)
```

**Creating Alternate Config:**
```
bluegreen_create_alt_config_directory
  ├─> bluegreen_detect_config_directory
  └─> bluegreen_copy_directory
      └─> bluegreen_copy_directory (recursive)
```

### Independent Functions (No Dependencies)

These functions don't call other bluegreen functions:
- `bluegreen_menu()`
- `bluegreen_permission()`
- `bluegreen_get_active_environment()`
- `bluegreen_get_environment_info()`
- `bluegreen_get_shared_tables()`
- `bluegreen_get_last_sync()`
- `bluegreen_set_last_sync()`
- `bluegreen_init()`
- `bluegreen_detect_config_directory()`

## Dependency Analysis

### Most Called Functions (Utility Functions)
1. **bluegreen_get_environment_info()** - Called by 3 functions
2. **bluegreen_detect_config_directory()** - Called by 2 functions
3. **bluegreen_get_config_directory()** - Called by 2 functions
4. **bluegreen_copy_directory()** - Called by 2 functions (plus itself)

### Most Dependent Functions (Complex Functions)
1. **bluegreen_is_configured()** - Calls 2 other functions
2. **bluegreen_sync_config_files()** - Calls 2 other functions
3. **bluegreen_create_alt_config_directory()** - Calls 2 other functions

### Recursive Functions
- **bluegreen_copy_directory()** - Recursively calls itself to copy subdirectories

## Function Categories by Purpose

### Data Retrieval (Read-only)
- `bluegreen_get_active_environment()`
- `bluegreen_get_environment_info()`
- `bluegreen_get_idle_environment()`
- `bluegreen_get_shared_tables()`
- `bluegreen_get_last_sync()`
- `bluegreen_get_table_count()`
- `bluegreen_environment_is_installed()`
- `bluegreen_detect_config_directory()`
- `bluegreen_get_config_directory()`
- `bluegreen_count_config_files()`

### Data Modification (Write operations)
- `bluegreen_set_last_sync()`
- `bluegreen_copy_directory()`
- `bluegreen_sync_config_files()`
- `bluegreen_create_alt_config_directory()`

### Validation/Check Functions
- `bluegreen_is_configured()`
- `bluegreen_environment_is_installed()`

### UI/Display Functions
- `bluegreen_menu()`
- `bluegreen_permission()`
- `bluegreen_init()`
- `bluegreen_admin_bar_output_build()`

---

**Note:** The diagrams above use Mermaid syntax and will render as interactive graphs on GitHub and other platforms that support Mermaid diagrams.
