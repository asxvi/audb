# Postgres Research

## Overview
This project is focused on building the extension for Postgres functions in GPROM (https://github.com/IITDBGroup/gprom). The extension will be used to generate the SQL queries for the Postgres functions. 

## Getting Started
1. Clone the repository:
    ```sh
    git clone <repository_url>
    ```
2. Navigate to the project directory:
    ```sh
    cd postgres_research
    ```
3. If you wish to test this independently of GPROM, set up the environment by creating a Postgres database and cloning this repository within the same directory:
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
    ```

4. Syntax for SQL Functions:

- Addition, Subtraction, Multiplication

```sh
SELECT range_set_add(
    ARRAY[ARRAY[-10, -10], ARRAY[3, 3]],
    ARRAY[ARRAY[-10, -10], ARRAY[20, 20]]
);
```

- Multiplication

```sh

```


## License
This project is licensed under the Apache 2.0 License. See the `LICENSE` file for more details.

