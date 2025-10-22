# Release Workflow

The release train is fully automated by GitHub Actions. The entry point is
`.github/workflows/release.yml`, which runs whenever `main` is updated (or via
`workflow_dispatch`). The jobs fire in the following order:

1. **`check_build`** inspects the diff for packaging-affecting paths. If the
   previous merge to `main` already produced wheels, the job can skip forcing a
   rebuild; otherwise it marks the next stage to rebuild artifacts.
2. **`check_release`** runs `python-semantic-release --noop` to decide whether a
   new semantic version should be cut. The version number is emitted as a job
   output so downstream steps share the result.
3. **`build`** reuses `./.github/workflows/build.yml` to invoke `cibuildwheel`
   and build the source distribution. The job only executes when either step 1
   requested a rebuild or step 2 detected a release-worthy change.
4. **`release`** runs on Ubuntu 22.04, downloads the wheel and sdist artifacts,
   runs `git-cliff` to prepend a changelog entry, and lets
   `python-semantic-release` publish the tag and PyPI release. A release PR is
   raised from `create-pull-request/patch` back to `main` so the generated
   changelog lands in the history.

## Local dry runs

Use these commands before landing changes that affect packaging or release:

```shell
# Preview the changelog range that git-cliff will generate
git cliff <previous-release>..HEAD --config cliff.toml --tag v<next-version> --output release-notes.md

# Mirror the semantic-release decision locally
uv run python -m semantic_release version --noop --verbosity DEBUG
```

If the changelog command emits no user-facing bullets, the automation will post
`- No user-facing changes.` instead of empty markdown.

## Triggering a release by hand

Kick off the workflow from the **Actions → Release → Run workflow** menu. The
`workflow_dispatch` input runs through the same steps as a commit to `main`,
including artifact build, `git-cliff`, and publishing. Double-check the
resulting PR titled `Release <X.Y.Z>` for the changelog diff before merging.
