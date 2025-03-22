#!/usr/bin/env bash

_rmc_options="--verbose --clear --help --reset --quiet --pid --cancel --kill --list --daemon --notify --name"

_rmc_complete_option() {
  COMPREPLY=($(compgen -W "$_rmc_options" -- "$2"))

  if [[ "${COMPREPLY[@]}" == "--name" ]]
  then
    COMPREPLY=("--name=")
    compopt -o nospace
  fi
}

_rmc_complete_name() {
  local names=$(rmc --list)
  COMPREPLY=($(compgen -W "$names" -- "$2"))
}

_rmc_completions() {
  if [[ "$3" == "--name" ]] || [[ "$3" = "=" ]]
  then
    _rmc_complete_name "$@"
  elif [[ "$2" =~ ^"-" ]]
  then
    _rmc_complete_option "$@"
  else
    COMPREPLY=($(compgen -A command "$2"))
  fi
}

complete -F _rmc_completions rmc
