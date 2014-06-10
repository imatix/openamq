#
#   textdb.pl - Flat-text database module
#                               
#   Copyright (c) 1991-2009 iMatix Corporation
#
#   ------------------ GPL Licensed Source Code ------------------
#   iMatix makes this software available under the GNU General
#   Public License (GPL) license for open source projects.  For
#   details of the GPL license please see www.gnu.org or read the
#   file license.gpl provided in this package.
#   
#   This program is free software; you can redistribute it and/or
#   modify it under the terms of the GNU General Public License as
#   published by the Free Software Foundation; either version 2 of
#   the License, or (at your option) any later version.
#   
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#   
#   You should have received a copy of the GNU General Public
#   License along with this program in the file 'license.gpl'; if
#   not, write to the Free Software Foundation, Inc., 59 Temple
#   Place - Suite 330, Boston, MA 02111-1307, USA.
#   
#   You can also license this software under iMatix's General Terms
#   of Business (GTB) for commercial projects.  If you have not
#   explicitly licensed this software under the iMatix GTB you may
#   only use it under the terms of the GNU General Public License.
#   
#   For more information, send an email to info@imatix.com.
#   --------------------------------------------------------------
#
#   textdb.pl   Select and sort rows in a flat-text database
#
#   Author: 98/10/19 Enrique Bengoechea <ebb@eco.uc3m.es>
#
#   This script is a minor modification of db-lib.pl and db_search.cgi
#   by Selena Sol (selena@eff.org, http://www.eff.org/~erict) and
#   Gunther Birznieks (birzniek@hlsun.redcross.org).
#
#   Here goes the copyright info for those scripts. The same applies
#   to this one:
#
#   Copyright Info: This library was written by Gunther Birznieks
#       (birzniek@hlsun.redcross.org) having been inspired by countless
#       other Perl authors. Feel free to copy, cite, reference, sample,
#       borrow, resell or plagiarize the contents.  However, if you don't
#       mind, please let me know where it goes so that I can at least
#       watch and take part in the development of the memes. Information
#       wants to be free, support public domain freware.  Donations are
#       appreciated and will be spent on further upgrades and other public
#       domain scripts.
#
#   You can easily generate the databases that can then be searched
#   in an Htmlpp document with Selena Sol's (free!) DataBase Manager CGI
#   <http://www.extropia.com/>


#   Subroutine db_query returns whether the current row
#   in the flat-text database matches the supplied criteria or not.

sub db_query
  {

        # Initially, the variables passed to this subroutine are
        # initialized as local variables.  $exact_match,
        # $case_sensitive, and *fields have been explained
        # above.
        #
        # $criteria, however, could use some explanation.
        #
        # Criteria elements are pipe delimited and consist
        # of 1) the value that the criteria will be
        # matched against, 2) the fields in the database
        # which will be matched against -note the first column is 1, not 0-
        # 3) the operator to use in  comparison, and finally,
        # 4) the data type that the
        # operator should use in the comparison (date, number, or
        # string comparison).
        #
        # Consider the following example
        #
        # @criteria = ("John|1|=|string");
        #  will return all rows correponding to people whose first name is
        #  "John", provided the first column in the database contains the first
        #  name of people.
        # @criteria = ("25|9|<=|number", "50|9|>=|number");
        #  will return all rows corresponding to people whose age is between
        #  25 and 50 years, provided column 9 in the database contains the age.
        #
        # In these examples, the subroutine will take the current
        # row and match it against the user-defined values for
        # <name>, <age_low> and <age_high>.  It will expect that the
        # <name> field will be field 1 and the age field will be
        # field 9.  It will perform a keyword match on the <name>
        # value and will perform range calculation on the <age>
        # field.
        #
        # As we are comparing user-defined criteria against the
        # actual database info, we will use several working
        # variables to define the 4 elements of each $criteria.
        # These will be $c_name, $c_fields, $c_op, $c_type
        #
        # @criteria_fields will collect those values in an array
        #
        # $not_found will flag us as to whether we found a match
        #
        # $month, $year, $day, $db_date, and $form_date are used
        # as date comparison place holders
        #
        # $db_index is a place marker for current database field
        # index we are looking at.
        #
        # @word_list is the list of words in a string for matching

  local($exact_match, $case_sensitive,
      *fields, $criteria) = @_;
  local($c_name, $c_fields, $c_op, $c_type);
  local(@criteria_fields);
  local($not_found);
  local($db_value);
  local($month, $year, $day);
  local($db_date, $form_date);
  local($db_index);
  local(@word_list);
  local $current_century = 20;

        # Now that we have defined our working environment, we are
        # going to take the current criteria element that we are
        # matching for and break it up into its 4 component parts.

  ($c_name, $c_fields, $c_op, $c_type) = split(/\|/, $criteria);

        # Next, we will perform a second split on the $c_fields
        # value because the criteria can match more than ONE
        # field in the database! For example, you may want to
        # allow the user to do a keyword match on several database
        # fields.
        #
        # Thus, we get the index values of the fields in each row
        # of the database that the variable will be compared
        # against. Note that these index numbers are comma
        # delimited.
        #
        # Remember, although fields and lists in perl start counting at 0,
        # when calling the subroutine $c_fields will start counting at 1,
        # because this is how later you will call fields in htmlpp syntax:
        # $(1), $(2), ...

  @criteria_fields = split(/,/,$c_fields);
  $index = 0;

  foreach (@criteria_fields){
      $criteria_fields[$index]--;
      $index++;
  }

        # Next, the subroutine gets the value of the form.
        # Once we have that, we are ready to apply the
        # criteria and the user-defined value to the actual
        # database. However, there are three cases of comparison
        # that will return a value each with its own slightly
        # different processing logic.
        #
        # Case 1: The field for the criteria was not filled
        # out, so the match is considered a success.
        #
        # Remember, if the user does not  enter a keyword, we want
        # the search to be open-ended. That is, we only restrict
        # the search if the user chooses to enter a search word
        # into the appropriate  query field.

  if ($c_name eq "")
    {
    return 0;
    }

        # Case 2: The data type is a number or a date OR the
        # data type is a string and the operator is NOT "=".  In
        # these cases, we match against the operator directly
        # based on the data type. (A string, "=" match is
        # considered a separate case below).
        #
        # Note that we use regular expressions to match the value
        # of $c_type to the values of date and number to determine
        # if it is the correct type of match.  We will use the or
        # (||) operator to make sure that if any of the conditions
        # are true, we will process them.

  if (($c_type =~ /date/i) ||
      ($c_type =~ /number/i) ||
      ($c_op ne "="))
   {
        # First, we set not_found to yes. We assume that the data
        # did not match. If any fields match the data submitted by
        # the user, then, we will set not_found to no later on
        # and we will know that we found a match.

    $not_found = "yes";

        # Next, the subroutine goes through each database field
        # specified in @criteria_fields and compares it to the
        # value provided in the query criterium.

    foreach $db_index (@criteria_fields)
      {
        # The first part of the comparison is to get the value of
        # the field in the database that corresponds to the
        # index number of the field we need to check.

      $db_value = $fields[$db_index];

        # Now we can go about comparing the user-submitted data to
        # the actual database field value.
        #
        # However, if the type of data comparison we are doing is
        # based on a date compare, then we need to convert the
        # date into the format YYYYMMDD instead of MM/DD/YY.
        #
        # This is because YYYYMMDD is easier to compare directly.
        # A date in the form YYYYMMDD can use the normal >,<,etc..
        # numerical operators to compare against.  Kind've a nifty
        # trick, huh?
        #
        # Besides flipping the date format around, we will also
        # format the date from 1 digit months and days (1 --> 01)
        # to 2 digit months and days and from 2 digit years to 4
        # digit years (87 --> 1987)

      if ($c_type =~ /date/i)
        {
        ($month, $day, $year) = split(/\//, $db_value);

        $month = "0" . $month
          if (length($month) < 2);

        $day = "0" . $day
          if (length($day) < 2);

        $year = ($current_century-1) . $year
          if (length($day) < 3);

        # Then we will assign the new formatted date to $db_date.

        $db_date = $year . $month . $day;

        # Next, we will perform the same type of reformatting on
        # the user-submitted date that we did for the date in the
        # database.

        ($month, $day, $year) = split(/\//, $c_name);

        $month = "0" . $month
          if (length($month) < 2);

        $day = "0" . $day
          if (length($day) < 2);

        $year = ($current_century-1) . $year
          if (length($day) < 3);

        # The user-submitted formatted date is stored in
        # $form_date

        $form_date = $year . $month . $day;

        # Now we can actually compare the date entered against the
        # date in the database.
        #
        # The subroutine supports all of the common comparison
        # operators (<, >, <=. >=, =, !=) and the comparisons work
        # as usual)
        #
        # If any of the date comparisons match then a 0 is
        # returned to let the submit_query routine know that a
        # match was found.

        if ($c_op eq ">")
      {
          return 0 if ($form_date > $db_date);
          }

        if ($c_op eq "<")
          {
          return 0 if ($form_date < $db_date);
      }

        if ($c_op eq ">=")
      {
          return 0 if ($form_date >= $db_date);
      }

        if ($c_op eq "<=")
      {
          return 0 if ($form_date <= $db_date);
      }

        if ($c_op eq "!=")
      {
          return 0 if ($form_date != $db_date);
      }

        if ($c_op eq "=")
      {
          return 0 if ($form_date == $db_date);
      }

      } # End of if ($c_type =~ /date/i)

        # If the data type is a number then we perform normal
        # number comparisons in Perl.

   elsif ($c_type =~ /number/i)
     {

     if ($c_op eq ">")
    {
        return 0 if ($c_name > $db_value);
    }

      if ($c_op eq "<")
    {
        return 0 if ($c_name < $db_value);
    }

      if ($c_op eq ">=")
    {
        return 0 if ($c_name >= $db_value);
    }

      if ($c_op eq "<=")
    {
        return 0 if ($c_name <= $db_value);
    }

      if ($c_op eq "!=")
    {
        return 0 if ($c_name != $db_value);
    }

      if ($c_op eq "=")
    {
        return 0 if ($c_name == $db_value);
    }
      } # End of elsif ($c_type =~ /number/i)

        # Finally, if the data type is a string then we take the
        # operators and apply the corresponding Perl string
        # operation. For example, != is ne, > is gt, etc.

    else
      {

      if ($c_op eq ">")
    {
        return 0 if ($c_name gt $db_value);
    }

      if ($c_op eq "<")
    {
        return 0 if ($c_name lt $db_value);
    }

      if ($c_op eq ">=")
    {
        return 0 if ($c_name ge $db_value);
    }

      if ($c_op eq "<=")
    {
        return 0 if ($c_name le $db_value);
    }

      if ($c_op eq "!=")
    {
        return 0 if ($c_name ne $db_value);
    }
      } # End of else $ct_type is a string.
    } # End of foreach $form_field
   } # End of case 2, Begin Case 3

        # Case 3: The data type is a string and the operator is =.
        # This is more complex because we need to check whether
        # our string matching matches whole words or is case
        # sensitive.
        #
        # In otherwords, this is a more "fuzzy" search.
        #
        # There are two special input form variables which might
        # afffect how we perform the search which have been
        # explained above: $exact_match, $case_sensitive
        #
        # In addition, the form_value will be split on whitespace
        # so that white-space separated words will be searched
        # separately.  Thus if the user submitted "hello world",
        # the subroutine would only match rows in which "hello"
        # and "world" appeared.

   else
     {
        # First, the subroutine takes the words that were entered
        # and parses them into an array of words based on word
        # boundary (\s+ splits on whitespace)

    @word_list = split(/\s+/,$c_name);

        # Again, we go through the fields in the database that are
        # checked for this  particular criteria definition.

    foreach $db_index (@criteria_fields)
      {
        # Also, as before, we obtain the value of the database
        # field we are currently matching against and set
        # $not_found equal to "yes".

      $db_value = $fields[$db_index];
      $not_found = "yes";

        # This time, however, we will use to new local
        # variables.  $match_word is a place marker for the words
        # we are going to be looking for in the database row.
        # $x is a place marker inside the for loops.

      local($match_word) = "";
      local($x) = "";

        # Now we begin searching for matches. Basically, the deal
        # is that as the words get found, they get removed
        # from the @word_list array.
        #
        # When the array is empty, we know that all the keywords
        # were found.
        #
        # We will later celebrate this event by returning the fact
        # that a match was found for this criteria.

      if ($case_sensitive eq "on")
    {
        if ($exact_match eq "on")
      {
          for ($x = @word_list; $x > 0; $x--)
        {
            $match_word = $word_list[$x - 1]; # \b matches on word boundary
            if ($db_value =~ /\b$match_word\b/)
          {
              splice(@word_list,$x - 1, 1);
              } # End of If
            } # End of For Loop
          }
        else
      {
          for ($x = @word_list; $x > 0; $x--)
        {
            $match_word = $word_list[$x - 1];
            if ($db_value =~ /$match_word/)
          {
              splice(@word_list,$x - 1, 1);
              } # End of If
            } # End of For Loop
          } # End of ELSE
      }
    else
      {
      if ($exact_match eq "on")
    {
        for ($x = @word_list; $x > 0; $x--)
      {
          $match_word = $word_list[$x - 1];
          if ($db_value =~ /\b$match_word\b/i)
        {
            splice(@word_list,$x - 1, 1);
            }
          } # End of for ($x = @word_list; $x > 0; $x--)
        } # End of if ($exact_match eq "on")
      else
    {
        for ($x = @word_list; $x > 0; $x--)
      {
      $match_word = $word_list[$x - 1];
          if ($db_value =~ /$match_word/i)
        {
            splice(@word_list,$x - 1, 1);
            }
          } # End of for ($x = @word_list; $x > 0; $x--)
        } # End of else
      } # End of else
    } # End of foreach $db_index

        # If there is nothing left in the word_list we want to say
        # that we found the word in the $db_value. Thus,
        # $not_found is set to "no" in this case.

    if (@word_list < 1)
      {
      $not_found = "no";
      }
  } # End of case 3

        # If not_found is still equal to yes, we return a 1,
        # indicating that the criteria was not satisfied
        #
        # If not_found is not yes, then we return that a
        # successful match was found (0).

  if ($not_found eq "yes")
    {
    return 1;
    }
  else
    {
    return 0;
    }
  } # End of sub db_query

######################################################################
#  Subroutine db_sort sorts the rows according to the field defined
#  to be sorted by.
######################################################################

sub db_sort {
        #
        # Remember that although arrays in Perl start counting from zero,
        # and this was also used in original Selena Sol's DB_Search,
        # here the argument to call db_sort must start counting fields
        # in 1, so that it's not somehow contrary to Htmlpp syntax
        # where you will call each field value as $(1), $(2), ...

  local(*database_rows, $sort_by) = @_;
  local $reverse_order = 0;
  local($join_delimiter, @row, @new_rows, $row, $new_row,
        @sorted_rows, $sorted_field, $sorted_row, $old_but_sorted_row);

        # Now here is where the real fun comes in.  We want to
        # sort the database rows that are displayed to the user.
        # The process of this is fairly simple.  For every
        # database row contained in @database_rows, we are going
        # to grab the value of the field defined as the field to
        # be sorted by and append that value to the very begining
        # of the line (so that the field will be repeated twice.)
        # Then you sort the rows (sort will sort on the first
        # characters first which is why you need to append the
        # sortable field to the front.)  Then, finally, you remove
        # the appended field so that the database rows are as they
        # began, but in a sorted order.
        #
        # Thus, if you were sorting by last name and you had the
        # following database rows ($row) in the @database_rows
        # array:
        #
        # Eric|Tachibanaerict@eff.org
        # Selena|Sol|selena@eff.org
        # Gunther|Birznieks|birzniek@hlsun.redcross.org
        #
        # The script would then take each row and append the last
        # name field to the front like so:
        #
        # Tachibana|Eric|Tachibana|erict@eff.org
        # Sol|Selena|Sol|selena@eff.org
        # Birznieks|Gunther|Birznieks|birzniek@hlsun.redcross.org

  $join_delimiter = $db_delimiter;
  $join_delimiter =~ s/\\// if ($db_delimiter =~ m/\\/);

        # If argument $sort_by contains the character "<" the sorting order
        # will be reversed (descending instead of ascending)

  if ($sort_by =~ m/</) {
      $sort_by =~ s/<//;
	  $sortby--;
      $reverse_order = 1;
  }

  foreach $row (@database_rows)
    {
    @row = split (/$db_delimiter/, $row);
    $sortable_field = $row[$sort_by];
    unshift (@row, $sortable_field);

    $new_row = join ("$join_delimiter", @row);
    push (@new_rows, $new_row);
    }

        # Once we have the rows reformatted as above, we are ready
        # to sort them.  First however, we erase the contents of
        # @database_rows since we are going to want to recreate
        # that array with the sorted rows from @new_rows in just a
        # moment.

  @database_rows = ();

        # Then we are ready to sort...guess what the result is:
        #
        # Birznieks|Gunther|Birznieks|birzniek@hlsun.redcross.org
        # Sol|Selena|Sol|selena@eff.org
        # Tachibana|Eric|Tachibana|erict@eff.org

  @sorted_rows = sort (@new_rows);

        # If reverse sorting is activated, reverse the sorted array
        # In this case the result would be:
        #
        # Tachibana|Eric|Tachibana|erict@eff.org
        # Sol|Selena|Sol|selena@eff.org
        # Birznieks|Gunther|Birznieks|birzniek@hlsun.redcross.org

  @sorted_rows = reverse (@sorted_rows) if ($reverse_order);

        # Next, we need remove that first sortable field so that
        # we have the following:
        #
        # Gunther|Birznieks|birzniek@hlsun.redcross.org
        # Selena|Sol|selena@eff.org
        # Eric|Tachibana|erict@eff.org
        #
        # Look!  They are now sorted by last name!  By the way, if
        # you sort by a field with numbers, remember that
        # computers sort with their own funky rules.  That is, if
        # you don't put a 0 before the number 1, it will sort after
        # 9 but alphabetical sorting should be just fine.

  foreach $sorted_row (@sorted_rows)
    {
    @row = split (/$db_delimiter/, $sorted_row);
    $sorted_field = shift (@row);
    $old_but_sorted_row = join ("$join_delimiter", @row);
    push (@database_rows, $old_but_sorted_row);
    }

        # now that we have sorted the rows, lets figure out how to
        # display them all.
        #

} # End of sub db_sort

1;
