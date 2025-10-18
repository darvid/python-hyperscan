#!/usr/bin/env bash
set -euo pipefail

if ! command -v git >/dev/null 2>&1; then
  echo "git is required" >&2
  exit 1
fi

if ! command -v git-cliff >/dev/null 2>&1; then
  echo "git-cliff is required (install via cargo or ./install.sh)" >&2
  exit 1
fi

if ! command -v gh >/dev/null 2>&1; then
  echo "GitHub CLI (gh) is required" >&2
  exit 1
fi

REPO="$(gh repo view --json nameWithOwner --jq '.nameWithOwner')"

COUNT="${1:-5}"
if ! [[ "${COUNT}" =~ ^[0-9]+$ ]]; then
  echo "Usage: $0 [count]" >&2
  exit 1
fi

REQUEST_LIMIT=$((COUNT + 1))
mapfile -t TAGS < <(gh release list --limit "${REQUEST_LIMIT}" --json tagName --jq '.[].tagName' 2>/dev/null || true)

if [[ "${#TAGS[@]}" -eq 0 ]]; then
  mapfile -t TAGS < <(git tag --list 'v[0-9]*' --sort=-creatordate | head -n "${REQUEST_LIMIT}")
fi

if [[ "${#TAGS[@]}" -eq 0 ]]; then
  echo "No release tags were found" >&2
  exit 1
fi

ROOT_COMMIT="$(git rev-list --max-parents=0 HEAD | tail -n 1)"
TEMP_DIR="$(mktemp -d)"
cleanup() {
  rm -rf "${TEMP_DIR}"
}
trap cleanup EXIT

for ((i = 0; i < COUNT && i < ${#TAGS[@]}; i++)); do
  tag="${TAGS[$i]}"
  current_commit="$(git rev-list -n 1 "${tag}")"
  prev_tag="${TAGS[$((i + 1))]:-}"
  range="${ROOT_COMMIT}..${tag}"

  if [[ -n "${prev_tag}" ]]; then
    range="${prev_tag}..${tag}"
  fi

  note_file="${TEMP_DIR}/${tag}.md"
  git cliff "${range}" --config cliff.toml --tag "${tag}" --output "${note_file}"

  if ! grep -q '^- ' "${note_file}"; then
    : >"${note_file}"
    echo "No user-facing changes detected; leaving ${tag} release notes empty."
  fi

  if ! gh release view "${tag}" >/dev/null 2>&1; then
    echo "Skipping ${tag}: release not found on GitHub"
    continue
  fi

  release_id="$(gh release view "${tag}" --json databaseId --jq '.databaseId')"

  echo "Updating ${tag} using ${note_file}"
  if [[ -s "${note_file}" ]]; then
    jq -n --arg body "$(cat "${note_file}")" '{body: $body}' | gh api --method PATCH "repos/${REPO}/releases/${release_id}" --input - >/dev/null
  else
    jq -n '{body: ""}' | gh api --method PATCH "repos/${REPO}/releases/${release_id}" --input - >/dev/null
  fi
done
