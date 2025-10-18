# Release Workflow

- Releases are prepared by `python-semantic-release` for version bumps while `git-cliff` (`cliff.toml`) builds the changelog entry and release notes.
- The automation computes the commit range between the previous `Release x` merge commit and the current head. Local dry runs can mirror this with `git cliff <previous-release>..HEAD --config cliff.toml --tag v<next-version> --output release-notes.md`.
- If the range has no user-visible commits, the workflow publishes a `- No user-facing changes.` note instead of letting GitHub generate filler text.
- During publishing, `softprops/action-gh-release` uploads artifacts with the generated notes so the GitHub release description and PyPI metadata stay in sync.
