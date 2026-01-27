# GPROM- AUDB Extension

## Overview
This project implements a range-based uncertainty extension for Postgres functions. Eventual implementation in GPROM (https://github.com/IITDBGroup/gprom).

## Getting Started
1. Clone the repository:
    ```sh
    git clone <repository_url>
    ```
2. Navigate to the project directory:
    ```sh
    cd audb/c_extension/i4r_audb_extension
    ```
    
    1. To make binary locally:
        ```sh
        make clean
        make && make install
        ```
     
    2. To run inside docker container
        ```sh
        docker compose up -d
        docker exec -it audb_test bash

        # Inside container
        make clean
        make && make install
        ```
    3. Connect to database
        ```sh
        psql -U postgres -d audb_test
        ```

3. Running Basic Test Suite
    ```sh
    # inside psql
    \i i4r_audb_extension_test--1.1.sql
    ```

4. Examples


<!-- 3. If you wish to test this independently of GPROM, set up the environment by creating a Postgres database and cloning this repository within the same directory:
- Create a Postgres database:
    ```sh
    createdb <database_name>
    ```
- Clone the repository
- Add functions to the database:
    ```sh
    psql -d <database_name> -f <path_to_functions.sql>
    ```
- Run the function to test:
    ```sh
    psql -d <database_name> -c "SELECT <function_name>(<arguments>);"
    ``` -->




## License
This project is licensed under the Apache 2.0 License. See the `LICENSE` file for more details.