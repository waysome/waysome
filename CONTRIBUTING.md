# Contributing guidelines for waysome

Never, ever, use a pseudonym. We only accept patches with real name.

Pull requests will _only_ be accepted on the development branch, called
'develop'. Only exceptions are hotfixes.

## Branches

We do not accept pull requests which are submitted from a 'master' branch. Use
topic branches.

Use the following naming scheme:

| Prefix    | use                                                    |
| --------- | ------------------------------------------------------ |
| add-      | Adding features                                        |
| remove-   | Removing stuff                                         |
| refactor- | Refactoring stuff, moving files, moving code, cleanup  |
| cleanup-  | Also refactoring stuff, but only code cleanup          |
| fix-      | Fixing something                                       |
| hotfix-   | Hotfixes which should go in as soon as possible        |

or similar. The dash should delimit the branch name, which should follow in
snake_case.

## Pull request contents

_Explain_ what you have done, implemented or changed. We will _not_ read code if
you did not add a sufficient description!

Do small changes. The more complex your changes are, the more commits you should
have. We do not accept one-commit-1k-changes pull requests.

If you don't know how to do commits or pullrequests properly, please use google
and read some articles about how to do things right. It saves a lot of effort,
also for you!

# Coding style

Well, this is not a styleguide. Just have a look at our code to learn the coding
style. Here are just some examples:

```C
    /**
     * here goes the brief explanation
     *
     * Here goes some more verbose documentation
     *
     * @return And the return documentation goes here
     */
    static struct some_type*
    my_static_function_with_no_args(void)
    {
        // ...
    }

    /**
     * See above
     */
    void
    r_my_nonstatic_function_with_no_args(void)
    {
        // ...
    }

    /**
     * See above
     */
    struct some_type*
    r_my_nonstatic_function_with_args(
            int a, //!< Argument explanation
            struct foobar** baz //!< Another argument
    ) {
        // ...
    }

    if (foo) {
        // then bar
    } else {
        // baz
    }

    switch(something) {
    case 1:
        // do something
        break;
    case 2:
        {
            // or something else, but in a block, if you need to
            break;
        }
    default:
        // do whatever
        break;
    }

    /**
     * Some documentation on the type
     */
    struct some_type
    {
        int a; //!< blabla
        int b; //!< bla blubb
    };
```

# Commits

When it comes to commits... we have some ideas how things should work:

## Commit message header

Explain what you have done in a commit in a few words. The long
description goes into the actual message, not the header

## Commit message body

Write as much as you want, but do not bore people. Be accurate and do not mess
things up. Be detailed but not excessive.

## Tagging your commits

We would like if you add a Signed-off-by: line to your commits. Further
explanation below.

An explanation of the Signed-off-by: tag can be found
[here](https://www.kernel.org/doc/Documentation/SubmittingPatches), point 12.

By adding the Signed-off-by: tag to your commit you agree on the following:

    Developer's Certificate of Origin 1.1

    By making a contribution to this project, I certify that:

    (a) The contribution was created in whole or in part by me and I
        have the right to submit it under the open source license
        indicated in the file; or

    (b) The contribution is based upon previous work that, to the best
        of my knowledge, is covered under an appropriate open source
        license and I have the right under that license to submit that
        work with modifications, whether created in whole or in part
        by me, under the same open source license (unless I am
        permitted to submit under a different license), as indicated
        in the file; or

    (c) The contribution was provided directly to me by some other
        person who certified (a), (b) or (c) and I have not modified
        it.

    (d) I understand and agree that this project and the contribution
        are public and that a record of the contribution (including all
        personal information I submit with it, including my sign-off) is
        maintained indefinitely and may be redistributed consistent with
        this project or the open source license(s) involved.

You are free of adding other tags, too. For more explanation and what tags are
feasible have a look at
[this](https://www.kernel.org/doc/Documentation/SubmittingPatches), points 13
and 14.

By adding you agree on the following:

    Reviewer's statement of oversight

    By offering my Reviewed-by: tag, I state that:

     (a) I have carried out a technical review of this patch to
         evaluate its appropriateness and readiness for inclusion into
         the project code base.

     (b) Any problems, concerns, or questions relating to the patch
         have been communicated back to the submitter.  I am satisfied
         with the submitter's response to my comments.

     (c) While there may be things that could be improved with this
         submission, I believe that it is, at this time, (1) a
         worthwhile modification to the codebase, and (2) free of known
         issues which would argue against its inclusion.

     (d) While I have reviewed the patch and believe it to be sound, I
         do not (unless explicitly stated elsewhere) make any
         warranties or guarantees that it will achieve its stated
         purpose or function properly in any given situation.

# More

If something was not mentioned here, have a look at
[this](https://www.kernel.org/doc/Documentation/SubmittingPatches), we want to
orientate on this kernel styleguide on how to submit patches.

## Contribute pizza!

If you want to contribute without writing code, testing or writing
documentation, feel free to order us some pizza. Some of the initial authors
don't drink alcohol, so we would prefer pizza over beer. Also: don't drink
and code.
